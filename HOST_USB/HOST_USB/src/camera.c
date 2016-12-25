/*
 * camera.c
 *
 * Created: 9/26/2016 4:43:00 PM
 *  Author: A41477
 */ 
#include "camera.h"
#include "wifiserial.h"
#include "led.h"


xTimerHandle xCameraCommTimeoutTimer = NULL;
xTimerHandle xCameraQUerytTimer = NULL;
xSemaphoreHandle startCameraProcessing = NULL;

CameraState_t cameraSendState = CAMERA_STATE_INIT;

// 2016/11 PA
uint16_t blockSeq = 0;
//PictureDataRsp_t  pictureData;
BlockSeqData pictureData;

#define UHI_CDC_BUFFER_SIZE (5*1024)

uint8_t recvBuf[UHI_CDC_BUFFER_SIZE];

extern xSemaphoreHandle startCameraProcessing;


typedef union PACK{
	struct PACK{
		uint8_t head;
		uint8_t type;
		uint8_t command;
		uint16_t sequenceNo;
		uint16_t blockSize;
	}req;
	uint8_t payload[50];
}CdcCmd_t;



CdcCmd_t snapshot;
CdcCmd_t getblockcommand;

typedef enum PACK{
	COMM_STATE_MACHINE_SOF      = 0,
	COMM_STATE_MACHINE_TYPE		= 1,
	COMM_STATE_MACHINE_ACK		= 2,
	COMM_STATE_MACHINE_SEQ0		= 3,
	COMM_STATE_MACHINE_SEQ1		= 4,
	COMM_STATE_MACHINE_LENGTH0  = 5,
	COMM_STATE_MACHINE_LENGTH1  = 6,
	COMM_STATE_MACHINE_DATA     = 7,
	COMM_STATE_MACHINE_PEC      = 8
}CommStateMachine_t;

CommStateMachine_t commStateMachine = COMM_STATE_MACHINE_SOF;
uint8_t checksum = 0;
uint16_t count = 0;
bool CameraPictureSnapshotReq(uint8_t command)
{
	uint8_t checksum = 0;
	/* Transfer UART RX fifo to CDC TX */
	if (!uhi_cdc_is_tx_ready(0)) {
		return false;
	} else {
		getblockcommand.req.head = COM_HEAD;
		getblockcommand.req.type = COM_CMD_TYPE_CMD;
		getblockcommand.req.command = command;//;
		if (command  == 0xff)
		{
			cameraSendState = CAMERA_STATE_INIT;
		} else if(command == 0x00)
		{
			cameraSendState = CAMERA_STATE_GET_SNAPSHOT;
		}
		
		uint8_t i = 0;
		for(i = 0; i < 3; i++ ){
			printf("%02x ",getblockcommand.payload[i]);
			checksum = FastCRC(checksum, getblockcommand.payload[i]);
		}
		getblockcommand.payload[i] = checksum;
		printf("%02x\r\n",checksum);
		uhi_cdc_write_buf(0, getblockcommand.payload, 3+1);
		//printf("SnapshotReq...\r\n");
	}
	return true;
}

// 2016/11 PA 
bool CameraPictureBlockReq(uint16_t blockSeq)
{
	uint8_t checksum = 0;
	if (!uhi_cdc_is_tx_ready(0)) {
		return false;
	} else {
		getblockcommand.req.head = COM_HEAD;
		getblockcommand.req.type = COM_CMD_TYPE_CMD;
		getblockcommand.req.command = CMD_GET_BLOCK_DATA;
		// 2016/11 PA 
		getblockcommand.req.sequenceNo = ((blockSeq%256)<<8) + (blockSeq / 256);//MSB
		getblockcommand.req.blockSize = ((DATA_BLOCK%256)<<8) + (DATA_BLOCK/256);//MSB
		uint8_t i = 0;
		for(i = 0; i < sizeof(getblockcommand.req); i++ ){
			printf("%02x ",getblockcommand.payload[i]);
			checksum = FastCRC(checksum, getblockcommand.payload[i]);
		}
		getblockcommand.payload[i] = checksum;
		printf("%02x\r\n",checksum);
		uhi_cdc_write_buf(0, getblockcommand.payload, sizeof(getblockcommand.req)+1);
	}
	return true;
}

void CameraPictureRxNotify(void)
{
	portBASE_TYPE higher_priority_task_woken = pdFALSE;
	if (uhi_cdc_is_rx_ready(0)) {
		xSemaphoreGiveFromISR(startCameraProcessing, &higher_priority_task_woken);
	}
}

void CameraPictureResetStateMachine(void)
{
	commStateMachine = COMM_STATE_MACHINE_SOF;
}

void vCameraCommTimeoutTimerCallback( xTimerHandle pxTimer )
{
	CameraPictureResetStateMachine();
}



void send_camera_block()
{
	static serial_out_pk_t send_packet;
	serial_out_pk_t *out_data = &send_packet;
	out_data->buf = &pictureData.picture_buff[0];
	out_data->len = pictureData.dataLength;
	xQueueSend(serial_out_queue, &out_data, 0);
}

void taskCamera( void *pvParameters)
{
	iram_size_t size = 0;
	/* Initialize the user interface */
	ui_init();

	/* Start USB host stack */
	uhc_start();
	vSemaphoreCreateBinary(startCameraProcessing);
	if (startCameraProcessing == NULL){
		printf("Failed to create Semaphore: startCameraProcessing \r\n");
	}
	for(;;) {
		xSemaphoreTake(startCameraProcessing, portMAX_DELAY);
		// reset recv buffer
		memset(recvBuf, 0, sizeof(recvBuf));
		size = 0;
		
		vPortEnterCritical();
		size = uhi_cdc_get_nb_received(0);
		uhi_cdc_read_buf(0, recvBuf, size);
		vPortExitCritical();
		printf("Receive data len=%d\r\n",size);

		if (cameraSendState < CAMERA_STATE_READ_DATA)
		{
			for (uint16_t i = 0; i< size; i++){
				printf("%02x ",recvBuf[i]);
			}
			printf("\r\n");
			if (recvBuf[0] == 0x8e)
			{
				if (recvBuf[1] == 0x01)// ACK frame
				{
					if (recvBuf[2] == 0x00)
					{
						if (cameraSendState == CAMERA_STATE_INIT)
						{
							printf("ACK frame init OK\r\n");
							CameraPictureSnapshotReq(0x00);
						} else if (cameraSendState == CAMERA_STATE_GET_SNAPSHOT) {
							printf("ACK frame get snapshot OK\r\n");
							CameraPictureBlockReq(0);
						}
					}
				}
				else if (recvBuf[1] == 0x02) { // Data frame
					if (size == 6) {
						pictureData.dataLength = (((char)recvBuf[ 4 ]) << 8) | (char)recvBuf[ 5 ];
						printf("Get data block is OK size=%d\r\n", pictureData.dataLength);
						cameraSendState = CAMERA_STATE_READ_DATA;
					}
				}// end Data frame
			}// if ==8e
		} else { // if <CAMERA_STATE_READ_DATA
			if (size < pictureData.dataLength) {
				printf("This block data is not full\r\n");
			} else {
				memset(pictureData.picture_buff, 0, sizeof(pictureData.picture_buff));
				memcpy(pictureData.picture_buff, recvBuf, pictureData.dataLength);
				printf("send data block out\r\n");
				send_camera_block();
			}
		}
	}// for loop

}
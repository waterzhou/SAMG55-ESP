/*
 * camera.c
 *
 * Created: 9/26/2016 4:43:00 PM
 *  Author: A41477
 */ 
#include "camera.h"
#include "led.h"
#include "string.h"


#define UHI_CDC_BUFFER_SIZE (5*1024)
#define DATA_BLOCK			(50*1024)
uint8_t*  recvBuf= NULL;
xSemaphoreHandle startCameraProcessing = NULL;
bool isInitACK = false;
uint8_t blockSeq = 0;
//#define PACK __attribute__ ((packed))

typedef union PACK{
#pragma pack (1)
	struct {
		uint8_t head;
		uint8_t type;
		uint8_t command;
		uint16_t sequenceNo;
		uint16_t blockSize;
	}req;
#pragma pack
	uint8_t payload[50];
}CdcCmd_t;



CdcCmd_t snapshot;
CdcCmd_t getblockcommand;


uint8_t checksum = 0;
uint16_t count = 0;

// CRC-8 x^8 + x^7 + x^6 + x^4 + x^2 + 1(0xD5)
static const uint8_t p_CRCtbl[256] = {
	0x00, 0xD5, 0x7F, 0xAA, 0xFE, 0x2B, 0x81, 0x54,
	0x29, 0xFC, 0x56, 0x83, 0xD7, 0x02, 0xA8, 0x7D,
	0x52, 0x87, 0x2D, 0xF8, 0xAC, 0x79, 0xD3, 0x06,
	0x7B, 0xAE, 0x04, 0xD1, 0x85, 0x50, 0xFA, 0x2F,
	0xA4, 0x71, 0xDB, 0x0E, 0x5A, 0x8F, 0x25, 0xF0,
	0x8D, 0x58, 0xF2, 0x27, 0x73, 0xA6, 0x0C, 0xD9,
	0xF6, 0x23, 0x89, 0x5C, 0x08, 0xDD, 0x77, 0xA2,
	0xDF, 0x0A, 0xA0, 0x75, 0x21, 0xF4, 0x5E, 0x8B,
	0x9D, 0x48, 0xE2, 0x37, 0x63, 0xB6, 0x1C, 0xC9,
	0xB4, 0x61, 0xCB, 0x1E, 0x4A, 0x9F, 0x35, 0xE0,
	0xCF, 0x1A, 0xB0, 0x65, 0x31, 0xE4, 0x4E, 0x9B,
	0xE6, 0x33, 0x99, 0x4C, 0x18, 0xCD, 0x67, 0xB2,
	0x39, 0xEC, 0x46, 0x93, 0xC7, 0x12, 0xB8, 0x6D,
	0x10, 0xC5, 0x6F, 0xBA, 0xEE, 0x3B, 0x91, 0x44,
	0x6B, 0xBE, 0x14, 0xC1, 0x95, 0x40, 0xEA, 0x3F,
	0x42, 0x97, 0x3D, 0xE8, 0xBC, 0x69, 0xC3, 0x16,
	0xEF, 0x3A, 0x90, 0x45, 0x11, 0xC4, 0x6E, 0xBB,
	0xC6, 0x13, 0xB9, 0x6C, 0x38, 0xED, 0x47, 0x92,
	0xBD, 0x68, 0xC2, 0x17, 0x43, 0x96, 0x3C, 0xE9,
	0x94, 0x41, 0xEB, 0x3E, 0x6A, 0xBF, 0x15, 0xC0,
	0x4B, 0x9E, 0x34, 0xE1, 0xB5, 0x60, 0xCA, 0x1F,
	0x62, 0xB7, 0x1D, 0xC8, 0x9C, 0x49, 0xE3, 0x36,
	0x19, 0xCC, 0x66, 0xB3, 0xE7, 0x32, 0x98, 0x4D,
	0x30, 0xE5, 0x4F, 0x9A, 0xCE, 0x1B, 0xB1, 0x64,
	0x72, 0xA7, 0x0D, 0xD8, 0x8C, 0x59, 0xF3, 0x26,
	0x5B, 0x8E, 0x24, 0xF1, 0xA5, 0x70, 0xDA, 0x0F,
	0x20, 0xF5, 0x5F, 0x8A, 0xDE, 0x0B, 0xA1, 0x74,
	0x09, 0xDC, 0x76, 0xA3, 0xF7, 0x22, 0x88, 0x5D,
	0xD6, 0x03, 0xA9, 0x7C, 0x28, 0xFD, 0x57, 0x82,
	0xFF, 0x2A, 0x80, 0x55, 0x01, 0xD4, 0x7E, 0xAB,
	0x84, 0x51, 0xFB, 0x2E, 0x7A, 0xAF, 0x05, 0xD0,
	0xAD, 0x78, 0xD2, 0x07, 0x53, 0x86, 0x2C, 0xF9,
};


uint8_t FastCRC(uint8_t LastCRC, uint8_t newbyte)
{
	LastCRC = p_CRCtbl[ LastCRC ^ newbyte ];
	return(LastCRC);
}

bool CameraPictureSnapshotReq(uint8_t command)
{
	if (command == 0xff)
		isInitACK = true;
	else if (command == 0x00)
		isInitACK = false;
	uint8_t checksum = 0;
	/* Transfer UART RX fifo to CDC TX */
	if (!uhi_cdc_is_tx_ready(0)) {
		return false;
	} else {
		memset(&getblockcommand,0, sizeof(getblockcommand));
		getblockcommand.req.head = COM_HEAD;
		getblockcommand.req.type = COM_CMD_TYPE_CMD;
		getblockcommand.req.command = command;//;
		printf("send:");
		uint8_t i = 0;
		for(i = 0; i < 3; i++ ){
			printf("0x%02x ",getblockcommand.payload[i]);
			checksum = FastCRC(checksum, getblockcommand.payload[i]);
		}
		getblockcommand.payload[i] = checksum;
		printf("%02x\r\n",checksum);
		uhi_cdc_write_buf(0, getblockcommand.payload, 3+1);
		//printf("SnapshotReq...\r\n");
	}
	return true;
}

bool CameraPictureBlockReq(uint8_t blockSeq)
{
	uint8_t checksum = 0;
	if (!uhi_cdc_is_tx_ready(0)) {
		return false;
	} else {
		memset(&getblockcommand,0, sizeof(getblockcommand));
		getblockcommand.req.head = COM_HEAD;
		getblockcommand.req.type = COM_CMD_TYPE_CMD;
		getblockcommand.req.command = CMD_GET_BLOCK_DATA;
		getblockcommand.req.sequenceNo = (uint16_t)blockSeq<<8;
		getblockcommand.req.blockSize = ((DATA_BLOCK%256)<<8) + (DATA_BLOCK/256);//MSB
		printf("send:");
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
		//cameraDataReceived = true;
		xSemaphoreGiveFromISR(startCameraProcessing, &higher_priority_task_woken);
	}
}



void CameraPictureDataHandler()
{
	vPortEnterCritical();
	iram_size_t size = uhi_cdc_get_nb_received(0);
	iram_size_t datalength = size;
	uhi_cdc_read_buf(0, recvBuf, size);
	vPortExitCritical();
	printf("recv(%d):",datalength);
	for (uint16_t i = 0; i < datalength; i ++)
	{
		printf("0x%2x ", recvBuf[i]);
	}
	printf("\r\n");
	// 0x8e 0x 1 0x 0 0x85 
	if (recvBuf[0] == 0x8e)
	{
		if(recvBuf[1] == 0x01)
		{
			if(recvBuf[2] == 0x00)
			{
				if(isInitACK)
				{
					//printf("This is init ACK OK frame\r\n");
					CameraPictureSnapshotReq(CMD_GET_SNAPSHOT);
				} else {
					//printf("This is get snap ACK OK frame\r\n");
					vTaskDelay(1000);
					blockSeq = 0;
					CameraPictureBlockReq(blockSeq);
				}
					
			}
		}
	}

}

void camera_task( void *pvParameters)
{
		if (recvBuf == NULL)
			recvBuf = malloc(DATA_BLOCK);
		vSemaphoreCreateBinary(startCameraProcessing);
		if (startCameraProcessing == NULL){
			printf("Failed to create Semaphore: startCameraProcessing \r\n");
		}
		for(;;) {
			xSemaphoreTake(startCameraProcessing, portMAX_DELAY);
			CameraPictureDataHandler();
		}
}
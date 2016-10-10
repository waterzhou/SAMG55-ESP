/*
 * temperature.c
 *
 * Created: 9/26/2016 4:43:30 PM
 *  Author: A41477
 */ 
#include "temperature.h"
#include "daikin/wifi/wifi_serial.h"

#define TSENSOR_UART_TIMEOUT_INTERVAL			(20  / portTICK_RATE_MS)

#define TSENSOR_DATA_QUERY_INIT_INTERVAL		(50    / portTICK_RATE_MS)
#define TSENSOR_DATA_QUERY_INTERVAL			    (5000  / portTICK_RATE_MS)
#define TSENSOR_NO_RSP_TIMEOUT_INTERVAL			(50000 / portTICK_RATE_MS)

xTimerHandle xTsensorCommTimeoutTimer = NULL;
xTimerHandle xTsensorNoRspTimeoutTimer = NULL;
xTimerHandle xTsensorDataQueryTimer = NULL;
xSemaphoreHandle startTsensorProcessing = NULL;

static uart_buff_t tempUartRecvBuff;
static uart_buff_t uartRecvBuff;
static volatile bool uartRecvDataStartHandle = false;

static uint8_t sendbuff[32];
ThermalParam_t thermalParam;
uint8_t thermimage[96*46 + 6];


bool tsensorQueryWaitForHandling = false;
TsensorDataQueryState_t tsensorDataQueryState = MEASUREMENT_INIT;
bool tsensorDataWaitforHandling = false;



static void uartDataParser(uint8_t data);
extern TsensorDataQueryState_t tsensorDataQueryState;

typedef enum {
	UART_STATE_MACHINE_SYMBOL_H     = 0,
	UART_STATE_MACHINE_MODE_HEAD	= 1,
	UART_STATE_MACHINE_SIZE0		= 2,
	UART_STATE_MACHINE_SIZE1		= 3,
	UART_STATE_MACHINE_DATA			= 4,
	UART_STATE_MACHINE_SYMBOL_T		= 5,
	UART_STATE_MACHINE_MODE_TAIL	= 6,
	UART_STATE_MACHINE_FINISHED		= 7
}UARTStateMachine_t;

UARTStateMachine_t uartStateMachine = UART_STATE_MACHINE_SYMBOL_H;

void tSensor_serial_init(void)
{
	uint32_t rx_timeout = (SERIAL_FRAME_INTERVAL * USART_BAUDRATE_TEM) / 1000;
	sam_usart_opt_t usart_settings = {
		.baudrate = USART_BAUDRATE_TEM,
		.char_length = USART_CHRL_TEM,
		.parity_type = USART_PARITY_TEM,
		.stop_bits = USART_NBSTOP_TEM,
		.channel_mode = US_MR_CHMODE_NORMAL
	};
	flexcom_enable(TSENSOR_SERIAL_PORT_FLEXCOM);
	flexcom_set_opmode(TSENSOR_SERIAL_PORT_FLEXCOM, FLEXCOM_USART);
	
	/* Configure USART */
	usart_init_rs232(TSENSOR_SERIAL_PORT, &usart_settings,
	sysclk_get_peripheral_hz());
	
	usart_set_rx_timeout(TSENSOR_SERIAL_PORT, rx_timeout);
	
	/* Enable USART1 RX interrupt. */
	NVIC_SetPriority(TSENSOR_SERIAL_PORT_IRQn, SERIAL_PORT_INT_PRIO);
	NVIC_EnableIRQ((IRQn_Type)TSENSOR_SERIAL_PORT_FLEXCOM_ID);
	usart_enable_interrupt(TSENSOR_SERIAL_PORT, (US_IER_TIMEOUT | US_IER_RXRDY));
	
	/* Enable the receiver and transmitter. */
	usart_start_rx_timeout(TSENSOR_SERIAL_PORT);
	usart_enable_tx(TSENSOR_SERIAL_PORT);
	usart_enable_rx(TSENSOR_SERIAL_PORT);
	
	/* Init thermo board only for once at start*/
	//Temp_Measure_Command_Send(INIT_SENSATION_MEASUREMENT);
}

static uint32_t recv_idx = 0;
void tSensor_uart_isr_handler(void)
{
	portBASE_TYPE higher_priority_task_woken = pdFALSE;
	uint32_t status;
	uint32_t symbol;
	Pdc *p_pdc = NULL;
	status = usart_get_status(TSENSOR_SERIAL_PORT);
	if(status & US_CSR_RXRDY) {
		if(usart_read(TSENSOR_SERIAL_PORT, &symbol) == 0) {
			if (tempUartRecvBuff.len < TSENSOR_RECV_BUFF_SIZE){
				tempUartRecvBuff.payload[tempUartRecvBuff.len++] = (uint8_t)symbol;
			}
		}
	}else if(status & US_CSR_TIMEOUT) {
		if (tempUartRecvBuff.len > 0){
			// notice task to process
			tsensorDataWaitforHandling = true;
			printf("Get data from sensor..........%d\r\n", tempUartRecvBuff.len);
			xSemaphoreGiveFromISR(startTsensorProcessing, &higher_priority_task_woken);
		}
		//tempUartRecvBuff.len = 0;
		usart_start_rx_timeout(TSENSOR_SERIAL_PORT);
	}else if(status & US_CSR_ENDTX) {
		p_pdc = usart_get_pdc_base(TSENSOR_SERIAL_PORT);
		pdc_disable_transfer(p_pdc, PERIPH_PTCR_TXTDIS);
		usart_disable_interrupt(TSENSOR_SERIAL_PORT, US_IDR_ENDTX);
	}else{
		/* Do nothing */
	}
}

void resetTsensorCommStateMachine(void)
{
	uartStateMachine = UART_STATE_MACHINE_MODE_HEAD;
}

void distributePacket(uint8_t* totalPacket, uint16_t len)
{
	if (len > 122)
	{
		uint16_t num = len / 122;
		if (len % 122 > 0)
		num += 1;
		for(uint16_t i = 0; i < num; i++)
		{
			printf("----------------------------------%d------------------\r\n", i);
			signal_to_wifi(totalPacket, i);
		}
	} else {
		signal_to_wifi(totalPacket, 0);
	}
}
uint8_t thermoIndex = 0;
void tSensor_handler(void)
{

	vPortEnterCritical();
	memcpy(&uartRecvBuff,&tempUartRecvBuff,tempUartRecvBuff.len + sizeof(uint16_t));
	tempUartRecvBuff.len = 0;
	vPortExitCritical();
	
	//xTimerStop(xTsensorCommTimeoutTimer, 0 );
	printf("%6d(%d):",xTaskGetTickCount(), uartRecvBuff.len);
	for (uint16_t i = 0; i < 3/*uartRecvBuff.len*/; i++){
		/*uartDataParser(uartRecvBuff.payload[i]); */printf("%02X ",uartRecvBuff.payload[i]);
	}
	printf("\r\n");
	if (uartRecvBuff.payload[0] == 0xCC &&
		uartRecvBuff.payload[1] == 0x80)
		{
			Temp_Measure_Command_Send(SENSATION_MEASUREMENT_STOP);
			memset(thermimage, 0, sizeof(thermimage));
			memcpy(&thermimage[0], &uartRecvBuff.payload[4], 96*46);
			/*printf("thermo image: ");
			for (uint16_t i = 0; i< 96*46; i++)
				printf("%02X ",thermimage[i]);
			printf("\r\n");*/
			//distributePacket(thermimage, 4416);
			thermoIndex = 0;
			signal_to_wifi(thermimage, thermoIndex++);
		
		}
	//xTimerStart(xTsensorCommTimeoutTimer, 0 );

}



static void uartDataParser(uint8_t data)
{
	static uint16_t count = 0;
	switch(uartStateMachine){
	case UART_STATE_MACHINE_SYMBOL_H:
		if (data == THERMAL_TO_G55_SYMBOL){
			uartStateMachine = UART_STATE_MACHINE_MODE_HEAD;
		}
		break;
	case UART_STATE_MACHINE_MODE_HEAD:
		if (   data == MODE_GRID_EYE_OUTPUT 
			|| data == MODE_CORRECTION_DATA_IMAGE 
			|| data == MODE_HIGH_RESULATION_THERMAL_IMAGE 
			|| data == MODE_HUMAN_DETECTION_RESULT
			|| data == MODE_COMMUNICATION_ERROR
			){
			thermalParam.mode = data;
			uartStateMachine = UART_STATE_MACHINE_SIZE0;
		}else{
			uartStateMachine = UART_STATE_MACHINE_SYMBOL_H;
		}
		break;
	case UART_STATE_MACHINE_SIZE0:
		thermalParam.size = data;
		uartStateMachine = UART_STATE_MACHINE_SIZE1;
		break;
	case UART_STATE_MACHINE_SIZE1:
		if(data != 0){
			thermalParam.size = thermalParam.size*data;
		}
		uartStateMachine = UART_STATE_MACHINE_DATA;
		count = 0;
		break;
	case UART_STATE_MACHINE_DATA:
		thermalParam.payload[count++] = data;
		if (count >= thermalParam.size){
			uartStateMachine = UART_STATE_MACHINE_SYMBOL_T;
		}
		break;
	case UART_STATE_MACHINE_SYMBOL_T:
		if (data == THERMAL_TO_G55_SYMBOL){
			uartStateMachine = UART_STATE_MACHINE_MODE_TAIL;
		}else{
			uartStateMachine = UART_STATE_MACHINE_SYMBOL_H;
		}
		break;
	case UART_STATE_MACHINE_MODE_TAIL:
		if (thermalParam.mode == MODE_GRID_EYE_OUTPUT){
			if (data == 0x3F){
				memcpy(thermalParam.gridEyeThermalImage.pixel,thermalParam.payload,sizeof(GridEyeThermalImage_t));
				uartStateMachine = UART_STATE_MACHINE_FINISHED;
				break;
			}
		}else if (thermalParam.mode == MODE_CORRECTION_DATA_IMAGE){
			if (data == 0x9F){
				memcpy(thermalParam.correctionDateImage.correctionDateImage,thermalParam.payload,sizeof(CorrectionDateImage_t));
				uartStateMachine = UART_STATE_MACHINE_FINISHED;
				break;
			}
		}else if (thermalParam.mode == MODE_HIGH_RESULATION_THERMAL_IMAGE){
			if (data == 0x8F){
				memcpy(thermalParam.highResolutionThermalImage.pixel,thermalParam.payload,sizeof(HighResolutionThermalImage_t));
				uartStateMachine = UART_STATE_MACHINE_FINISHED;
				break;
			}
		}else if (thermalParam.mode == MODE_HUMAN_DETECTION_RESULT){
			if (data == 0x9F){
				memcpy((uint8_t *)&thermalParam.humanDetectionInfo[0],thermalParam.payload,thermalParam.size);
				uartStateMachine = UART_STATE_MACHINE_FINISHED;
				break;
			}
		}else if (thermalParam.mode == MODE_COMMUNICATION_ERROR){
			if (data == 0xEF){
				memcpy((uint8_t *)&thermalParam.communicationError,thermalParam.payload,sizeof(CommunicationError_t));
				uartStateMachine = UART_STATE_MACHINE_FINISHED;
				break;
			}
		}
		uartStateMachine = UART_STATE_MACHINE_SYMBOL_H;
		break;
	case UART_STATE_MACHINE_FINISHED:
		// Received Finished ready for handling
		break;
	default:
		uartStateMachine = UART_STATE_MACHINE_SYMBOL_H;
		break;
	}
}

void sendToTsensorUart(uint8_t *buff, int32_t buffSize)
{
	Pdc *p_pdc = NULL;
	pdc_packet_t packet;
	p_pdc = usart_get_pdc_base(TSENSOR_SERIAL_PORT);
	
	packet.ul_addr = (uint32_t)buff;
	packet.ul_size = buffSize;
	
	pdc_tx_init(p_pdc, &packet, NULL);
	pdc_enable_transfer(p_pdc, PERIPH_PTCR_TXTEN);
	usart_enable_interrupt(TSENSOR_SERIAL_PORT, US_IER_ENDTX);
}

void Temp_Measure_Command_Send(uint8_t command)
{
	sendbuff[0] = G55_TO_THERMAL_SYMBOL;
	sendbuff[1] = 0x10;
	sendbuff[2] = 0x01;
	sendbuff[3] = 0x00;
	sendbuff[4] = command;
	sendbuff[5] = 0xAA;
	sendbuff[6] = 0x1F;
	sendToTsensorUart(sendbuff, 7);
}

void Temp_Measure_Get_Air_Condition_Info(uint8_t roomTemperature, uint8_t roomHumidity)
{
	sendbuff[0] = G55_TO_THERMAL_SYMBOL;
	sendbuff[1] = 0x88;
	sendbuff[2] = 0x02;
	sendbuff[3] = 0x00;
	sendbuff[4] = roomTemperature;//
	sendbuff[5] = roomHumidity;//
	sendbuff[6] = 0xAA;
	sendbuff[7] = 0x8F;
	sendToTsensorUart(sendbuff, 8);
}

void sendback_thermo_image_data ()
{
	/*static uint8_t resp_buf[8];
	uint8_t *p = &resp_buf[0];
	static serial_out_pk_t resp_send_packet;
	static serial_out_pk_t *resp_out_data = &resp_send_packet;

	*p++ = SERIAL_SOF;
	*p++ = ENCRYPT_MODE;
	*p++ = 2;
	*p++ = CUSTOMIZE_CMD_DEV_CTRL_GET_TEMP_RSP;
	*p++ = temperature;
	*p = sum8(&resp_buf[0], p - &resp_buf[0]);
	p++;
	resp_out_data->buf = resp_buf;
	resp_out_data->len = p - resp_buf;
	IoT_xQueueSend(serial_out_queue, &resp_out_data, 1000);*/
	//nm_uart_send(UART1, &buf[0], p - &buf[0]);
}

void vTsensorCommTimeoutTimerCallback( xTimerHandle pxTimer )
{
	resetTsensorCommStateMachine();
}

void vxTsensorDataQueryTimerCallback( xTimerHandle pxTimer )
{
	printf("Trigger semaphore for sensor2..................\r\n");
	xSemaphoreGive( startTsensorProcessing );
	//tsensorQueryWaitForHandling = true;
}

void vTsensorNoRspTimeoutTimerCallback( xTimerHandle pxTimer )
{
	xTimerChangePeriod( xTsensorDataQueryTimer, TSENSOR_DATA_QUERY_INIT_INTERVAL, 0 );
	tsensorDataQueryState = MEASUREMENT_INIT;
}

void sensor_task( void *pvParameters)
{
	vSemaphoreCreateBinary(startTsensorProcessing);
	if (startTsensorProcessing == NULL){
		printf("Failed to create Semaphore: startTsensorProcessing \r\n");
	}
	
	xTsensorCommTimeoutTimer = xTimerCreate("TTTimer", TSENSOR_UART_TIMEOUT_INTERVAL , pdFALSE, ( void * ) 0, vTsensorCommTimeoutTimerCallback);
	if(xTsensorCommTimeoutTimer == NULL ){
		printf("Failed to create xTsensorCommTimeoutTimer \r\n");
	}
	
	xTsensorDataQueryTimer = xTimerCreate("QTimer", TSENSOR_DATA_QUERY_INIT_INTERVAL , pdTRUE, ( void * ) 0, vxTsensorDataQueryTimerCallback);
	if(xTsensorDataQueryTimer != NULL ){
		//xTimerStart(xTsensorDataQueryTimer, 0 );
	}else{
		printf("Failed to create xTsensorDataQueryTimer \r\n");
	}
	
	xTsensorNoRspTimeoutTimer = xTimerCreate("TNSPTimer", TSENSOR_NO_RSP_TIMEOUT_INTERVAL , pdFALSE, ( void * ) 0, vTsensorNoRspTimeoutTimerCallback);
	if(xTsensorNoRspTimeoutTimer == NULL ){
		printf("Failed to create xTsensorDataQueryTimer \r\n");
	}else{
		
	}
	
	for(;;) {
		//vTaskDelay(1000);
		xSemaphoreTake(startTsensorProcessing, portMAX_DELAY);
		//printf("taskTSensor invoked\r\n");
		/*if (tsensorQueryWaitForHandling){
			tsensorQueryWaitForHandling = false;
			switch(tsensorDataQueryState){
				case MEASUREMENT_INIT:
				Temp_Measure_Command_Send(INIT_SENSATION_MEASUREMENT);
				printf("%06d:MEASUREMENT_INIT\r\n",xTaskGetTickCount());
				tsensorDataQueryState= MEASUREMENT_START;
				xTimerStart(xTsensorNoRspTimeoutTimer, 0 );// start received data state
				break;
				case MEASUREMENT_START:
				Temp_Measure_Command_Send(SENSATION_MEASUREMENT_START);
				printf("%06d:MEASUREMENT_START\r\n",xTaskGetTickCount());
				tsensorDataQueryState= MEASUREMENT_INIT_QUERY_AIR_INFO;
				break;
				case MEASUREMENT_INIT_QUERY_AIR_INFO:
				Temp_Measure_Get_Air_Condition_Info(0x00, 50);
				printf("%06d:MEASUREMENT_INIT_QUERY_AIR_INFO\r\n",xTaskGetTickCount());
				tsensorDataQueryState= MEASUREMENT_QUERY_AIR_INFO;
				xTimerChangePeriod( xTsensorDataQueryTimer, TSENSOR_DATA_QUERY_INTERVAL, 0 );
				break;
				case MEASUREMENT_QUERY_AIR_INFO:
				Temp_Measure_Get_Air_Condition_Info(0x00, 50);
				printf("%06d:MEASUREMENT_QUERY_AIR_INFO\r\n",xTaskGetTickCount());
				break;
				case MEASUREMENT_STOP:
				Temp_Measure_Command_Send(SENSATION_MEASUREMENT_STOP);
				printf("%06d:MEASUREMENT_STOP\r\n",xTaskGetTickCount());
				break;
				default:
				tsensorDataQueryState= MEASUREMENT_INIT;
				break;
			}
		}*/
		if (tsensorDataWaitforHandling){
			//printf("%06d:Received\r\n",xTaskGetTickCount());
			tsensorDataWaitforHandling = false;
			//xTimerStop(xTsensorNoRspTimeoutTimer, 0 );
			//xTimerStart(xTsensorNoRspTimeoutTimer, 0 );
			tSensor_handler();
		}
		
	}
}
/*
 * temperature.c
 *
 * Created: 9/26/2016 4:43:30 PM
 *  Author: Waterzhou
 */ 
#include "temperature.h"
#include "wifiserial.h"

#define TSENSOR_UART_TIMEOUT_INTERVAL			(20  / portTICK_RATE_MS)

#define TSENSOR_DATA_QUERY_INIT_INTERVAL		(50    / portTICK_RATE_MS)
#define TSENSOR_DATA_QUERY_INTERVAL			    (5000  / portTICK_RATE_MS)
#define TSENSOR_NO_RSP_TIMEOUT_INTERVAL			(50000 / portTICK_RATE_MS)

xTimerHandle xTsensorCommTimeoutTimer = NULL;
xTimerHandle xTsensorNoRspTimeoutTimer = NULL;
xTimerHandle xTsensorDataQueryTimer = NULL;
xSemaphoreHandle startTsensorProcessing = NULL;

static uart_buff_t tempUartRecvBuff;
//static uart_buff_t uartRecvBuff;
static volatile bool uartRecvDataStartHandle = false;

static uint8_t sendbuff[32];

uint8_t thermimage[96*46 + 6];


bool tsensorQueryWaitForHandling = false;
TsensorDataQueryState_t tsensorDataQueryState = MEASUREMENT_INIT;

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
	
	tempUartRecvBuff.payload = thermimage;
}


void tSensor_uart_isr_handler(void)
{
	portBASE_TYPE higher_priority_task_woken = pdFALSE;
	uint32_t status;
	uint32_t symbol;
	Pdc *p_pdc = NULL;
	status = usart_get_status(TSENSOR_SERIAL_PORT);
	#if 1
	if(status & US_CSR_RXRDY) {
		if(usart_read(TSENSOR_SERIAL_PORT, &symbol) == 0) {
			if (tempUartRecvBuff.len < TSENSOR_RECV_BUFF_SIZE){
				tempUartRecvBuff.payload[tempUartRecvBuff.len++] = (uint8_t)symbol;
			}
		}
	}else if(status & US_CSR_TIMEOUT) {
		if (tempUartRecvBuff.len > 0){
			// notice task to process
			//printf("Get data from sensor..........%d\r\n", tempUartRecvBuff.len);
			printf("%6d(%d)\r\n",xTaskGetTickCount(), tempUartRecvBuff.len);
			//memcpy(&uartRecvBuff,&tempUartRecvBuff,tempUartRecvBuff.len + sizeof(uint16_t));
			tempUartRecvBuff.len = 0;
			if (tempUartRecvBuff.payload[0] == 0xCC &&
				tempUartRecvBuff.payload[1] == 0x80)
				{
					xSemaphoreGiveFromISR(startTsensorProcessing, &higher_priority_task_woken);
				}
			
		}

		usart_start_rx_timeout(TSENSOR_SERIAL_PORT);

	} else if(status & US_CSR_ENDTX) {
		p_pdc = usart_get_pdc_base(TSENSOR_SERIAL_PORT);
		pdc_disable_transfer(p_pdc, PERIPH_PTCR_TXTDIS);
		usart_disable_interrupt(TSENSOR_SERIAL_PORT, US_IDR_ENDTX);
	}
	#endif
}

uint8_t thermoIndex = 0;
void tSensor_handler(void)
{
	Temp_Measure_Command_Send(SENSATION_MEASUREMENT_STOP);
	printf("thermo image: ");
	/*for (uint16_t i = 0; i< 96*46; i++)
		printf("%02X ",thermimage[i]);
	printf("\r\n");*/
	thermoIndex = 0;
	signal_to_wifi(&thermimage[4], thermoIndex++);
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

void sensor_task( void *pvParameters)
{
	tSensor_serial_init();
	vSemaphoreCreateBinary(startTsensorProcessing);
	if (startTsensorProcessing == NULL){
		printf("Failed to create Semaphore: startTsensorProcessing \r\n");
	}
	for(;;) {
		//vTaskDelay(1000);
		xSemaphoreTake(startTsensorProcessing, portMAX_DELAY);
		tSensor_handler();	
	}
}
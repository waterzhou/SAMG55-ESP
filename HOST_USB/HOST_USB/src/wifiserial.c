/*
 * wifiserial.c
 *
 * Created: 2016/12/10 9:18:52
 *  Author: WaterZhou
 */ 

#include <string.h> 
#include "asf.h"
#include "config/rtos.h"
#include "wifiserial.h"
#include "temperature.h"
#include "camera.h"

xQueueHandle serial_in_queue = NULL;
xQueueHandle serial_out_queue = NULL;

static xTimerHandle xConfigTimer;
static xTimerHandle xLedModeTimer;

static serial_in_pk_t serial_pk0;
static serial_in_pk_t serial_pk1;
static serial_in_pk_t *serial_recving = NULL;
static serial_in_pk_t *serial_recved = NULL;
static uint32_t recv_idx = 0;

uint8_t serial_buf_test[256];
uint8_t url_buf[128];

typedef struct _data_upload {
	uint8_t cmd_index;
	uint8_t value;
}data_upload_t;

typedef struct _uart_setting {
	uint8_t baud_index;
	uint8_t flow_ctrl;
}data_uart_cfg_t;


#define ON					1
#define OFF					0

static uint8_t led_state = OFF;
static data_uart_cfg_t uart_cfg_cmd;

typedef struct {
	unsigned int timestamp;
	unsigned char	data[];
}dataupload_t;

VIRTUAL_DEV g_virtual_dev;

#define ENTER_CONFIG_MODE				0
#define ENTER_GENERAL_MODE				1
#define ENTER_APP_OTAU_MODE				2
#define ENTER_WIFI_FW_OTAU_MODE			3

#define LED_MODE_NONE                  0
#define LED_MODE_CONNECT               1
#define LED_MODE_OFF				   2
#define LED_MODE_ON					   3
#define LED_MODE_OTAU				   4

uint8_t led_blinking_mode = LED_MODE_OFF;
uint8_t uart_ready = 0;
volatile int uart_beatheart = 0;
extern uint8_t thermoIndex;
extern uint8_t thermimage[96*46 + 6];

void wifi_module_reset(void)
{
	//ioport_set_pin_level(WINC_PIN_CHIP_ENABLE, IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(ESP_PIN_RESET, IOPORT_PIN_LEVEL_LOW);
	delay_ms(100);
	//ioport_set_pin_level(WINC_PIN_CHIP_ENABLE, IOPORT_PIN_LEVEL_HIGH);
	delay_ms(100);
	ioport_set_pin_level(ESP_PIN_RESET, IOPORT_PIN_LEVEL_HIGH);
	delay_ms(100);
}

static int byte2hexstrstr(const uint8_t *pBytes, uint32_t srcLen, uint8_t *pDstStr, uint32_t dstLen)
{
	const char tab[] = "0123456789abcdef";
	uint32_t i = 0;

	memset(pDstStr, 0, dstLen);

	if (dstLen < srcLen * 2)
	srcLen = (dstLen - 1) / 2;

	for (i = 0; i < srcLen; i++)
	{
		*pDstStr++ = tab[*pBytes >> 4];
		*pDstStr++ = tab[*pBytes & 0x0f];
		pBytes++;
	}
	*pDstStr++ = 0;
	return srcLen * 2;
}

void wifi_serial_init(uint32_t baudspeed)
{
	//uint32_t rx_timeout = (SERIAL_FRAME_INTERVAL * baudspeed) / 1000;
	uint32_t rx_timeout = (2 * baudspeed) / 1000;
	sam_usart_opt_t usart_settings = {
		.baudrate = baudspeed,
		.char_length = USART_CHRL,
		.parity_type = USART_PARITY,
		.stop_bits = USART_NBSTOP,
		.channel_mode = US_MR_CHMODE_NORMAL
	};
	flexcom_enable(WIFI_SERIAL_PORT_FLEXCOM);
	flexcom_set_opmode(WIFI_SERIAL_PORT_FLEXCOM, FLEXCOM_USART);
	
	/* Configure USART */
	//usart_init_rs232(WIFI_SERIAL_PORT, &usart_settings,sysclk_get_peripheral_hz());
	usart_init_hw_handshaking(WIFI_SERIAL_PORT, &usart_settings,sysclk_get_peripheral_hz());
	
	usart_set_rx_timeout(WIFI_SERIAL_PORT, rx_timeout);
	
	/* Enable USART1 RX interrupt. */
	NVIC_SetPriority(WIFI_SERIAL_PORT_IRQn, SERIAL_PORT_INT_PRIO);
	NVIC_EnableIRQ((IRQn_Type)WIFI_SERIAL_PORT_FLEXCOM_ID);
	usart_enable_interrupt(WIFI_SERIAL_PORT, (US_IER_TIMEOUT | US_IER_RXRDY));
	
	serial_recving = &serial_pk0;
	serial_recved = &serial_pk1;
	
	/* Enable the receiver and transmitter. */
	usart_start_rx_timeout(WIFI_SERIAL_PORT);
	usart_enable_tx(WIFI_SERIAL_PORT);
	usart_enable_rx(WIFI_SERIAL_PORT);

}

void WIFI_SERIAL_PORT_HANDLER(void)
{
	uint32_t status;
	uint32_t symbol;
	Pdc *p_pdc = NULL;
	serial_in_pk_t *ptemp = NULL;
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	status = usart_get_status(WIFI_SERIAL_PORT);
	//printf("USART6_Handler\r\n");
	if(status & US_CSR_RXRDY) {
		if(usart_read(WIFI_SERIAL_PORT, &symbol) == 0) {
			if(recv_idx < MAXIMUM_DATA_LENGTH) {
				serial_recving->buf[recv_idx] = (uint8_t)symbol;
				recv_idx++;
			}
		}
	}
	else if(status & US_CSR_TIMEOUT) {
		/*
		IoT_xTimerStartFromISR(serial_tmr, &xHigherPriorityTaskWoken);
		if(xHigherPriorityTaskWoken != pdFALSE) {
			IoT_vPortYieldFromISR();
		}
		*/
		ptemp = serial_recving;
		serial_recving = serial_recved;
		serial_recved = ptemp;
		serial_recved->len = recv_idx;

		recv_idx = 0;
		usart_start_rx_timeout(WIFI_SERIAL_PORT);
		//IoT_DEBUG(IoT_DBG_ON | IoT_DBG_INFO, ("Wifi receive data timeout............\r\n"));
		xQueueSendFromISR(serial_in_queue, &serial_recved, &xHigherPriorityTaskWoken);
		if(xHigherPriorityTaskWoken != pdFALSE) {
			vPortYieldFromISR();
		}
	}
	else if(status & US_CSR_ENDTX) {
		p_pdc = usart_get_pdc_base(WIFI_SERIAL_PORT);
		pdc_disable_transfer(p_pdc, PERIPH_PTCR_TXTDIS);
		usart_disable_interrupt(WIFI_SERIAL_PORT, US_IDR_ENDTX);
	}
	else if(status &US_CSR_ENDRX){
		printf("........endrx\r\n");
		/* Do nothing */
	}
}

unsigned char sum8(unsigned char *A, unsigned char n)
{
	unsigned char i;
	unsigned char checksum = 0;
	for(i = 0; i < n; i++)
	{
		checksum += A[i];
	}
	return(checksum);
}



static uint16_t form_serial_packet(uint8_t cmdid, uint8_t *data, uint8_t datalen, uint8_t *buf)
{
	uint8_t *p = buf;

	*p++ = SERIAL_SOF;
	*p++ = ENCRYPT_MODE;
	*p++ = datalen + 1;
	*p++ = cmdid;
	if((data != NULL) && (datalen > 0)) {
		memcpy(p, data, datalen);
		p = p + datalen;
	}
	*p = sum8(buf, (p - buf));
	p++;

	return (p - buf);
}

static void serial_resp_out(uint8_t resp_id, uint8_t status)
{
	static uint8_t resp_buf[8];
	uint8_t *p = &resp_buf[0];
	static serial_out_pk_t resp_send_packet;
	static serial_out_pk_t *resp_out_data = &resp_send_packet;

	*p++ = SERIAL_SOF;
	//*p++ = ENCRYPT_MODE;
	*p++ = 2;
	*p++ = resp_id;
	*p++ = status;
	*p = sum8(&resp_buf[0], p - &resp_buf[0]);
	p++;
	resp_out_data->buf = resp_buf;
	resp_out_data->len = p - resp_buf;
	xQueueSend(serial_out_queue, &resp_out_data, 1000);
	//nm_uart_send(UART1, &buf[0], p - &buf[0]);
}

void signal_to_wifi(uint8_t *data, uint8_t index)
{
	static uint8_t resp_buf[256];
	uint8_t *p = &resp_buf[0];
	static serial_out_pk_t resp_send_packet;
	static serial_out_pk_t *resp_out_data = &resp_send_packet;

	*p++ = SERIAL_SOF;
	*p++ = ENCRYPT_MODE;
	*p++ = 124;//数据长度 + 命令字1 + index
	*p++ = 0x8a;
	*p++ = index;
	if((data != NULL)) {
		memcpy(p, data + index*122, 122);
		p = p + 122;
	}
	*p = sum8(&resp_buf[0], p - resp_buf);
	p++;
	resp_out_data->buf = resp_buf;
	resp_out_data->len = p - resp_buf;
	xQueueSend(serial_out_queue, &resp_out_data, 1000);
}





/* Write UUID information into Wi-Fi module */
void config_wifi_module(void)
{
	static serial_out_pk_t send_packet;
	serial_out_pk_t *out_data = &send_packet;
	static uint8_t pkt_buf[16];
	uint16_t pkt_len;
	uint8_t uuid[6] = {'4','K','D','3','R','C'};
	//uint8_t uuid[6] = {'W','3','E','P','5','9'};
	
	pkt_len = form_serial_packet(CMD_DEVICE_UUID, uuid, 6, pkt_buf);
	out_data->buf = pkt_buf;
	out_data->len = pkt_len;
	xQueueSend(serial_out_queue, &out_data, portMAX_DELAY);
}

extern xSemaphoreHandle startTsensorProcessing;
#if 0
static void sendback_temperature ()
{
	static uint8_t resp_buf[1024];
	//memset(resp_buf, 0xff, 256);
	for(int i = 0; i<1024; i++)
		resp_buf[i] = i;
	uint8_t *p = &resp_buf[0];
	static serial_out_pk_t resp_send_packet;
	static serial_out_pk_t *resp_out_data = &resp_send_packet;
	resp_out_data->buf = resp_buf;
	resp_out_data->len = 400;
	xQueueSend(serial_out_queue, &resp_out_data, 1000);
	//nm_uart_send(UART1, &buf[0], p - &buf[0]);*/
}
#endif


static void startTemperature(void)
{
	printf("Receive get temperature command.\r\n");
	Temp_Measure_Command_Send(INIT_SENSATION_MEASUREMENT);
	delay_ms(500);
	Temp_Measure_Command_Send(SENSATION_MEASUREMENT_START);
	delay_ms(500);
	Temp_Measure_Get_Air_Condition_Info(0x00, 50);
}

static void startPicture(void)
{
	printf("Receive get snapshot command.\r\n");
	CameraPictureSnapshotReq(0xff);
}

static void execute_serial_cmd(uint8_t cmdid, uint8_t *data, uint8_t datalen)
{
	uint16_t len;
	uint8_t *p = NULL;
	
	switch(cmdid)
	{
		case CMD_WIFI_MODULE_READY:
		{
			printf("Wi-Fi Module Ready!\r\n");
			uart_ready = 1;
			led_blinking_mode = LED_MODE_ON;
			break;
		}
		case CUSTOMIZE_CMD_DEV_CTRL_GET_TEMP:
			startTemperature();

		break;
		case CUSTOMIZE_CMD_DEV_GET_BLOCK:
			printf("Get camera specific block---%d\r\n", data[0]);
			CameraPictureBlockReq(data[0]);
		break;
		
		case CUSTOMIZE_CMD_DEV_CTRL_GET_PIC:
			startPicture();
		break;
#if 1 //for temperaature
		case CUSTOMIZE_CMD_DEV_TEMP_RSP:
			printf("get wifi res for image\r\n");
			if (thermoIndex < 38)
				signal_to_wifi(&thermimage[4], thermoIndex++);
		break;
#endif
		
		case CMD_PACKET_ERROR_RESP:
			printf("Receive packet error response, err(%d).\r\n", *data);
		break;
		
		case CUSTOMIZE_CMD_STATUS_REPORT:
		{
			uint8_t states = *data;
			if(states == 0) {
				printf("Wifi disconnect.\r\n");
			}
			else if(states == 1) {
				printf("Wifi module in sniffer mode.\r\n");
			}
			else if (states == 2) {
				printf("Wifi module connect to wifi router.\r\n");
			}
			else if(states == 3) {
				printf("Wifi module connect to JD server.\r\n");
			}
			//states = 0;
			//signal_to_wifi(CUSTOMIZE_CMD_STATUS_REPORT_RESP, &states, 1);
		}
		break;
		
		case CMD_CONNECT_RESP:
			printf("Connect to Remote Server OK.\r\n");
		break;
		
		case CMD_CONNECTION_BEATHEART:
			uart_beatheart++;
			printf("Connection beatheart.\r\n");
		break;
		
		case CUSTOMIZE_CMD_DATA_UPLOAD_RESP:
			printf("Update Data OK.\r\n");
		break;
		
		case CUSTOMIZE_CMD_FACTORY_RESET_RESP:
		{
			LED_On(LED0);
			led_state = ON;
			printf("Reset to FactoryNew OK.\r\n");
			
		}
		break;
		
		case CMD_DEVICE_UUID_RESP:
			if(*data == CMD_SUCCESS) {
				//Wi-Fi module will connect the remote server automatically after it gets the UUID.
				//start_wifi_connect();
				//config_wifi_app_otau_url();
				printf("UUID RESP OK.\r\n");
			}
			else {
				printf("UUID RESP Error.\r\n");
			}
		break;
		
		case CMD_START_SNIFFER_RESP:
		{
			printf("Start sniffer mode...\r\n");
			break;
		}
		
		case CMD_GOT_SSID_PSK_RESP:
		{
			p = data + 1;
			len = strlen(p);
			printf("Got wifi ssid: %s, psk: %s\r\n", p, p + len + 1);
			break;
		}
		
		case CMD_START_WIFI_CONNECT_RESP:
		{
			p = data + 1;
			len = strlen(p);
			printf("Start wifi connect ssid(%s), psk(%s).\r\n", p, p + len + 1);
			break;
		}
		
		case CMD_WIFI_CLOUD_READY:
		{
			printf("Wi-Fi connect to Cloud OK\r\n");
			break;
		}
		case CUSTOMIZE_CMD_CHANGE_UART_CFG_RESP:
		{
			printf("Wi-Fi return get uart cfg OK\r\n");
			if(uart_cfg_cmd.baud_index == 1)
			{
				wifi_serial_init(BIT_RATE_19200);
			}
			break;
		}

		
		case CMD_UDP_PACKET_UNKNOWN_PACKET:
			printf("UDP packet unknown packet.\r\n");
		break;
		
		case CMD_OUT_OF_MEMORY:
			printf("Error: out of memory.\r\n");
		break;
		
		case CUSTOMIZE_CMD_FACTORY_TEST_RESP:
			if(*data == 0x0) {
				printf("AP can Found.\r\n");
			}
			else {
				printf ("AP not Found.\r\n");
			}
		break;
		
		case CMD_UART_TIMEOUT:
			printf("Error: UART timeout.\r\n");
		break;
		
		default:
			printf("Unsupported command(%d).\r\n", cmdid);
		break;

	}
}

void parse_serial_packet(uint8_t *buf, uint8_t buflen)
{
	uint8_t *p = buf;
	uint8_t *data = NULL;
	uint8_t resp = CMD_SUCCESS;
	uint8_t len, crc, cmdid;
	
	byte2hexstrstr(buf, buflen, serial_buf_test, 256);
	printf ("Serial IN(%d): %s\r\n", buflen, serial_buf_test);
	
	while(buflen > 3) {
		if(*p != SERIAL_SOF) {
			if (uart_ready == 0)
				return;
			resp = CMD_INVALID_HEAD;
			printf("Invalid header received (0x%x).\r\n", *p);
			serial_resp_out(CMD_PACKET_ERROR_RESP, resp);
			return;
		}
	
		len = *(p + 1 + 1) + 3;

		crc = sum8(p, len);

		if(*(p + len) != crc) {
			resp = CMD_CRC_ERROR;
			printf("Invalid CRC, Received CRC(0x%x), Calculated CRC(0x%x).\r\n", *(p + len), crc);
			//serial_resp_out(CMD_PACKET_ERROR_RESP, CMD_CRC_ERROR);
			return;
		}
		cmdid = *(p + 3);
		data = p + 4;
		len = *(p + 1 + 1) - 1;
		execute_serial_cmd(cmdid, data, len);
		//in case two packet coming together
		len = *(p + 1 + 1) + 4;
		p = p + len;
		buflen = buflen - len;
	}
	return;
}

void wifi_in(void *parameter)
{
	serial_in_pk_t *in_data = NULL;
	wifi_module_reset();
	printf("wifi_in task started\r\n");

	for(;;) {
		xQueueReceive(serial_in_queue, &in_data, portMAX_DELAY);
		parse_serial_packet(in_data->buf, in_data->len);
	}
}

static void vLedModeCallback( xTimerHandle pxTimer )
{
	switch(led_blinking_mode){
		case LED_MODE_CONNECT:
		{
			LED_Toggle(LED1);
			led_state = led_state^1;
			break;
		}
		case LED_MODE_OTAU:
		{
			LED_Toggle(LED1);
			led_state = led_state^1;
			break;
		}
		case LED_MODE_OFF:
		{
			LED_Off(LED1);
			led_state = 0;
			led_blinking_mode = LED_MODE_NONE;
			break;
		}
		case LED_MODE_ON:
		{
			LED_On(LED1);
			led_state = 1;
			led_blinking_mode = LED_MODE_NONE;
			break;
		}
		default:
		break;
	}
	//sendback_temperature ();
}

static void vConfigModeCallback( xTimerHandle pxTimer )
{
	static uint8_t count;
	static uint8_t button_mode = ENTER_GENERAL_MODE;
	
	static uint8_t pkt_buf[16];
	static uint16_t pkt_len;

	static serial_out_pk_t send_packet;
	serial_out_pk_t *out_data = &send_packet;
	
	
	vPortEnterCritical();
	count++;
	vPortExitCritical();
	
	if(!ioport_get_pin_level(BUTTON_0_PIN)){
		if(count >= 5){
			printf("enter config mode\r\n");
			button_mode = ENTER_CONFIG_MODE;
			led_blinking_mode = LED_MODE_ON;
		}
		else {
			printf("enter test command mode\r\n");
			button_mode = ENTER_GENERAL_MODE;
		}
	}
	else{
		//button released, exit FN mode
		if(button_mode == ENTER_CONFIG_MODE){
			printf("perform config mode\r\n");
			led_blinking_mode = LED_MODE_CONNECT;
			pkt_len = form_serial_packet(CUSTOMIZE_CMD_FACTORY_RESET, NULL, 0, pkt_buf);
			out_data->buf = pkt_buf;
			out_data->len = pkt_len;
			xQueueSend(serial_out_queue, &out_data, 0);
		}
		else if (button_mode == ENTER_GENERAL_MODE){
			printf("perform test command mode\r\n");	
			startPicture();
		}
		vPortEnterCritical();
		count = 0;
		vPortExitCritical();
		button_mode = ENTER_GENERAL_MODE;
		xTimerStop(xConfigTimer, 0);
	}
}

void wifi_task(void *parameter)
{
	(void) parameter;
	Pdc *p_pdc = NULL;
	pdc_packet_t packet;
	serial_out_pk_t *out_data = NULL;

	
	xConfigTimer = xTimerCreate("xConfigTimer", 1000 , pdTRUE, ( void * ) 0, vConfigModeCallback);
	if(xConfigTimer == NULL ){
		printf("xConfigTimer create failed.\r\n");
	}
	
	xLedModeTimer = xTimerCreate("xLedTimer", 2500 , pdTRUE, ( void * ) 0, vLedModeCallback);
	if(xLedModeTimer != NULL ){
		xTimerStart(xLedModeTimer, 0 );
	}
	else{
		printf("xLedModeTimer create failed.\r\n");
	}

	printf("serial_out task started\r\n");
	

	serial_in_queue = xQueueCreate(SERIAL_IN_QUEUE_LEN, sizeof(void *));
	if(serial_in_queue == NULL) {
		printf("Serial Queue In create failed\r\n");
		//while(true);
	}
	serial_out_queue = xQueueCreate(SERIAL_OUT_QUEUE_LEN, sizeof(void *));
	if(serial_out_queue == NULL) {
		printf("Serial Queue Out create failed\r\n");
		//while(true);
	}
	
	xTaskCreate(wifi_in, "wifi_in", WIFI_RECV_TASK_STACK_SIZE, NULL, WIFI_RECV_TASK_PRIORITY, NULL);

	for(;;) {

		p_pdc = usart_get_pdc_base(WIFI_SERIAL_PORT);
		
		/* Pended here if no message received */
		xQueueReceive(serial_out_queue, &out_data, portMAX_DELAY);

		printf("Serial OUT(%d)\r\n", out_data->len);

		packet.ul_addr = (uint32_t)out_data->buf;
		packet.ul_size = out_data->len;
		
		pdc_tx_init(p_pdc, &packet, NULL);
		pdc_enable_transfer(p_pdc, PERIPH_PTCR_TXTEN);
		usart_enable_interrupt(WIFI_SERIAL_PORT, US_IER_ENDTX);

	}
}

void vFNBtton_Click_Hook( void )
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	
	xTimerStartFromISR(xConfigTimer, &xHigherPriorityTaskWoken );
	if(xHigherPriorityTaskWoken != pdFALSE) {
		vPortYieldFromISR();
	}
	
}
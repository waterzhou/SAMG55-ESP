/**
 * \file
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */


#ifndef SERIAL_H_
#define SERIAL_H_

/* Serial port configuration */
typedef enum {
	BIT_RATE_300     = 300,
	BIT_RATE_600     = 600,
	BIT_RATE_1200    = 1200,
	BIT_RATE_2400    = 2400,
	BIT_RATE_4800    = 4800,
	BIT_RATE_9600    = 9600, //index = 0
	BIT_RATE_19200   = 19200, //index = 1
	BIT_RATE_38400   = 38400,
	BIT_RATE_57600   = 57600,
	BIT_RATE_74880   = 74880,
	BIT_RATE_115200  = 115200,
	BIT_RATE_230400  = 230400,
	BIT_RATE_460800  = 460800,
	BIT_RATE_921600  = 921600,
	BIT_RATE_1843200 = 1843200,
	BIT_RATE_3686400 = 3686400,
} UART_BautRate; //you can add any rate you need in this range

#define WIFI_SERIAL_PORT					USART0

//#define USART_BAUDRATE						BIT_RATE_9600
#define USART_PARITY						US_MR_PAR_NO
#define USART_CHRL							US_MR_CHRL_8_BIT
#define USART_NBSTOP						US_MR_NBSTOP_1_BIT

#define WIFI_SERIAL_PORT_FLEXCOM_ID			ID_FLEXCOM0
#define WIFI_SERIAL_PORT_FLEXCOM			FLEXCOM0
#define WIFI_SERIAL_PORT_IRQn				FLEXCOM0_IRQn
#define WIFI_SERIAL_PORT_HANDLER			FLEXCOM0_Handler

#define MAXIMUM_DATA_LENGTH					1024
#define SERIAL_PORT_INT_PRIO				14



#define WIFI_RECV_TASK_STACK_SIZE			1024
#define WIFI_RECV_TASK_PRIORITY				tskIDLE_PRIORITY + 5

#define SERIAL_FRAME_INTERVAL				10 //10ms

#define SERIAL_SOF							0x7E
#define ENCRYPT_MODE						0

/* cmd id */
//#define CUSTOMIZE_CMD_UUID_READ				0X01
//#define CUSTOMIZE_CMD_UUID_READ_RESP		0X81
#define CUSTOMIZE_CMD_CHANGE_UART_CFG		0X01
#define CUSTOMIZE_CMD_CHANGE_UART_CFG_RESP		0X81
#define CUSTOMIZE_CMD_FACTORY_RESET			0X02
#define CUSTOMIZE_CMD_FACTORY_RESET_RESP	0X82
#define CUSTOMIZE_CMD_STATUS_REPORT			0X03
#define CUSTOMIZE_CMD_STATUS_REPORT_RESP	0X83
#define CUSTOMIZE_CMD_FACTORY_TEST			0X04
#define CUSTOMIZE_CMD_FACTORY_TEST_RESP		0X84
#define CUSTOMIZE_CMD_GET_SNAPSHOT			0X08
#define CUSTOMIZE_CMD_GET_SNAPSHOT_RESP		0X88
#define CUSTOMIZE_CMD_DATA_UPLOAD			0X09
#define CUSTOMIZE_CMD_DATA_UPLOAD_RESP		0X89
#define CUSTOMIZE_CMD_DEV_CTRL_GET_TEMP		0X0A
#define CUSTOMIZE_CMD_DEV_CTRL_GET_TEMP_RSP	0X8A
#define CUSTOMIZE_CMD_DEV_CTRL_GET_PIC		0X0B
#define CUSTOMIZE_CMD_RECIPE_DOWN_RESP		0X8B

#define CMD_PACKET_ERROR_RESP				(0x80 + 0x20) // Serial frame error response
#define CMD_CONNECT							(0x01 + 0x20)
#define CMD_CONNECT_RESP					(0x81 + 0x20)
#define CMD_FACTORY_RESET					(0x02 + 0x20)
#define CMD_FACTORY_RESET_RESP				(0x82 + 0x20)
#define CMD_DATA_UPDATA						(0x03 + 0x20)
#define CMD_DATA_UPDATA_RESP				(0x83 + 0x20)
#define CMD_DEVICE_CONTROL					(0x04 + 0x20)
#define CMD_DEVICE_CONTROL_RESP				(0x84 + 0x20)
#define CMD_DEVICE_STATUS_QUERY				(0x05 + 0x20)
#define CMD_DEVICE_STATUS_QUERY_RESP		(0x85 + 0x20)
#define CMD_DEVICE_UUID						(0x06 + 0x20)
#define CMD_DEVICE_UUID_RESP				(0x86 + 0x20)
#define CMD_REQ_APP_OTAU					(0x07 + 0x20)
#define CMD_REQ_APP_OTAU_RESP				(0x87 + 0x20)
#define CMD_REQ_WIFI_FW_OTAU				(0x08 + 0x20)
#define CMD_REQ_WIFI_FW_OTAU_RESP			(0x88 + 0x20)
#define CMD_REQ_SET_APP_OTAU_URL			(0x09 + 0x20)
#define CMD_REQ_SET_APP_OTAU_URL_RESP		(0x89 + 0x20)
#define CMD_REQ_SET_WIFI_FW_OTAU_URL		(0x0A + 0x20)
#define CMD_REQ_SET_WIFI_FW_OTAU_URL_RESP	(0x8A + 0x20)
#define CMD_REQ_AP_SCAN_TEST				(0X0B + 0x20)
#define CMD_REQ_AP_SCAN_TEST_RESP			(0X8B + 0x20)

#define CMD_START_SNIFFER_RESP				0xFE
#define CMD_GOT_SSID_PSK_RESP				0XFD
#define CMD_START_WIFI_CONNECT_RESP			0XFC
#define CMD_START_CONNECT_TO_SERVER			0XFB
#define CMD_CONNECTION_BEATHEART			0XFA
#define CMD_WIFI_MODULE_READY				0XF9
#define CMD_WIFI_CLOUD_READY				0XF8

#define APP_OTA_URL							"https://s3-us-west-1.amazonaws.com/mo.ismail/m2m_ota_3a0.bin"
//#define APP_OTA_URL_CRT						"https://s3-us-west-1.amazonaws.com/mo.ismail/cortus_app_ota.bin"
#define APP_OTA_URL_CRT						"http://storage.360buyimg.com/devh5/1451282851757_cortus_app_ota.bin"

/* cmd status */
#define CMD_SUCCESS					0
#define CMD_INVALID_HEAD			1
#define CMD_CRC_ERROR				2
#define CMD_UNSUPPORTED				3
#define CMD_EXECUTE_ERROR			4
#define CMD_PARAMETER_ERROR			5
#define CMD_BUFFER_INSUFFICIENT		6
#define CMD_INVALID_URL				7
#define CMD_OTAU_DL_FAILED			8
#define CMD_OTAU_SW_FAILED			9
#define CMD_AP_NOT_FOUND			0xff

#define SERIAL_TX_COMPLETED			1

#define SERIAL_IN_QUEUE_LEN			1
#define SERIAL_OUT_QUEUE_LEN		1

//FOR TEST
#define CMD_ACCESSKEY_NOT_FOUND				0XC0
#define CMD_UDP_PACKET_RECEIVE				0XC1
#define CMD_UDP_SCAN_PACKET					0XC2
#define CMD_UDP_CONFIG_PACKET_V0			0XC3
#define CMD_UDP_CONFIG_PACKET_V1			0XC4
#define CMD_UDP_CONTROL_PACKET				0XC5
#define CMD_UDP_SEND_FAIL					0XC6
#define CMD_UDP_RECV_ERROR					0XC7
#define CMD_UDP_RECV_BUF_NULL				0XC8
#define CMD_UDP_PACKET_ANALYZE_ERROR		0XC9
#define CMD_UDP_PACKET_JSONCONTROL_PACKET	0XCA
#define CMD_UDP_PACKET_UNKNOWN_PACKET		0XCB
#define CMD_OUT_OF_MEMORY					0XCC
#define CMD_UART_TIMEOUT					0XCD


typedef struct virtual_dev {   // real device update this Structure parameters
	uint8_t power;
	uint8_t work_mode;
	uint8_t temp_value;
	uint8_t light_value;
	uint8_t time_delay;
} VIRTUAL_DEV;

typedef struct _serial_in_pk {
	/* data buffer to Ethernet */
	uint8_t buf[MAXIMUM_DATA_LENGTH];
	/* data length */
	int32_t len;
}serial_in_pk_t;

typedef struct _serial_out_pk {
	/* data buffer to Serial port */
	uint8_t *buf;
	/* data length */
	int32_t len;
	/* Return value */
	int32_t ret;
}serial_out_pk_t;

extern xQueueHandle serial_in_queue;
extern xQueueHandle serial_out_queue;
void wifi_task(void *parameter);
void wifi_in(void *parameter);
void wifi_serial_init(uint32_t baudspeed);

#endif /* SERIAL_H_ */
/*
 * download.c
 *
 * Created: 3/10/2015 2:24:05 PM
 *  Author: water.zhou
 */ 
#include "asf.h"
#include "config/rtos.h"
#include "misc/debug.h"
#include "download.h"
#include "firmware.h"
#include <string.h>
#define RX_BUFFER_SIZE          (1024)

xQueueHandle serial_in_queue = NULL;
xQueueHandle serial_out_queue = NULL;
xQueueHandle serial_out_ret_queue = NULL;
static uint32_t recv_idx = 0;
static serial_in_pk_t *serial_recving = NULL;
static serial_in_pk_t *serial_recved = NULL;
static serial_out_pk_t serial_pk0;
static serial_in_pk_t serial_pk1;
/* Counts the number of times the Rx task receives a string.  The count is used
to ensure the task is still executing. */
static uint32_t rx_task_loops = 0UL;
/* A buffer into which command outputs can be written is declared here, rather
than in the command console implementation, to allow multiple command consoles
to share the same buffer.  For example, an application may allow access to the
command interpreter by UART and by Ethernet.  Sharing a buffer is done purely
to save RAM.  Note, however, that the command console itself is not re-entrant,
so only one command interpreter interface can be used at any one time.  For that
reason, no attempt at providing mutual exclusion to the cOutputBuffer array is
attempted. */
static int8_t cOutputBuffer[400];
static const uint8_t *const welcome_message = (uint8_t *) "SAMG55&WILC1000B: send command\r\n\r\n>";
/* The USART instance used for input and output. */
static freertos_usart_if cli_usart = NULL;

/** Usart download read task */
void usart_download_rd_tsk(void *pvParameters)
{
	freertos_usart_if usart_port;
	uint8_t rx_buffer;
	uint32_t received;
	unsigned portBASE_TYPE string_index;

	/* The (already open) USART port is passed in as the task parameter. */
	usart_port = (freertos_usart_if)pvParameters;

	string_index = 0;
	serial_out_pk_t *out_data = &serial_pk0;

	for (;;) {
		memset(rx_buffer, 0x00, sizeof(rx_buffer));

		received = freertos_usart_serial_read_packet(usart_port, &rx_buffer,
				sizeof(rx_buffer),portMAX_DELAY);
		if (received > 0) {
			printf("0x%x ",rx_buffer);
			out_data->buf = &rx_buffer;
			out_data->len = sizeof(rx_buffer);
			IoT_xQueueSend(serial_out_queue, &out_data, 0);
		}
	}
}

/** Usart download write task */
void usart_download_wr_tsk(void *pvParameters)
{
	/* Reserve 300 pages **/
	uint32_t ul_fw_page_addr = (IFLASH_ADDR + IFLASH_SIZE - IFLASH_PAGE_SIZE * 300);
	uint8_t *output_string;
	uint32_t ul_rc, buflen = 0;
	serial_out_pk_t *receive_data = NULL;
	portTickType max_block_time_ticks = 200UL / portTICK_RATE_MS;
	cli_usart = (freertos_usart_if) pvParameters;
	IoT_DEBUG(DOWNLOAD_DBG | IoT_DBG_INFO, ("Download Write Task Started...\r\n"));
	
	output_string = cOutputBuffer;
	/* Send the welcome message.  The message is copied into RAM first as the
	DMA cannot send from Flash addresses. */
	strcpy((char *) output_string, (char *) welcome_message);
	freertos_usart_write_packet(cli_usart, output_string,
						strlen((char *)welcome_message), max_block_time_ticks);
	
	/* Initialize flash: 6 wait states for flash writing. */
	ul_rc = flash_init(FLASH_ACCESS_MODE_128, 6);
	if (ul_rc != FLASH_RC_OK) {
		IoT_DEBUG(DOWNLOAD_DBG | IoT_DBG_INFO, ("Flash init error...\r\n"));
		while(true);
	}
	ul_rc = flash_unlock(ul_fw_page_addr,
	ul_fw_page_addr + IFLASH_PAGE_SIZE * 300 - 1, 0, 0);
	if (ul_rc != FLASH_RC_OK) {
		IoT_DEBUG(DOWNLOAD_DBG | IoT_DBG_INFO, ("Flash Unlock error %lu...\r\n",(unsigned long)ul_rc));
		while(true);
	}
	serial_out_queue = IoT_xQueueCreate(SERIAL_OUT_QUEUE_LEN, sizeof(void *));
	if(serial_out_queue == NULL) {
		IoT_DEBUG(DOWNLOAD_DBG | IoT_DBG_SERIOUS, ("Serial Queue Out create failed\r\n"));
		while(true);
	}
	
	for (;;)
	{
		/* Pended here if no message received */
		IoT_xQueueReceive(serial_out_queue, &receive_data, portMAX_DELAY);
		IoT_DEBUG(DOWNLOAD_DBG | IoT_DBG_SERIOUS, ("Writing flash=0x%x \r\n", *(receive_data->buf)));

		if (buflen % IFLASH_PAGE_SIZE == 0) {
		/* Flash erase sector **/
		ul_rc = flash_erase_sector(ul_fw_page_addr + buflen);
			if (ul_rc != FLASH_RC_OK) {
				IoT_DEBUG(DOWNLOAD_DBG | IoT_DBG_INFO, ("Flash Erase sector error %lu...\r\n",(unsigned long)ul_rc));
				while(true);
			}
		}
		/* Flash Write**/
		while (*((uint8_t *)(ul_fw_page_addr + buflen)) != *(receive_data->buf))
		{
			ul_rc = flash_write(ul_fw_page_addr + buflen, receive_data->buf,receive_data->len, 0);
			if (ul_rc != FLASH_RC_OK) {
				IoT_DEBUG(DOWNLOAD_DBG | IoT_DBG_INFO, ("Flash Write error %lu...\r\n",(unsigned long)ul_rc));
				while(true);
			}
		}
		//IoT_DEBUG(DOWNLOAD_DBG | IoT_DBG_INFO, ("check 0x%x...\r\n",(unsigned long)*(ul_fw_page_addr + buflen)));
		buflen += receive_data->len;
		/* Flash lock**/
		/*
		ul_rc = flash_lock(ul_fw_page_addr,
		ul_fw_page_addr + IFLASH_PAGE_SIZE*300 - 1, 0, 0);
		if (ul_rc != FLASH_RC_OK) {
			printf("-F- Flash locking error %lu\n\r", (unsigned long)ul_rc);
			return 0;
		}*/
	}

}





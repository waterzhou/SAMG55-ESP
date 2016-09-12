/**
 * \file
 *
 * \brief SSL implementation.
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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


#ifndef CONF_SERVER_H
#define CONF_SERVER_H

#define SSL_SERVER_NAME				"live.smart.jd.com"
//#define SSL_SERVER_NAME				"111.206.227.37"
#define SSL_SERVER_PORT				2001
//#define SSL_SERVER_NAME				"apismart.jd.com"
//#define SSL_SERVER_PORT				443

#define SSL_READ_STACK				1000
#define SSL_READ_PRIORITY			tskIDLE_PRIORITY + 2

#define SSL_WRITE_STACK				1000
#define SSL_WRITE_PRIORITY			tskIDLE_PRIORITY + 3

#define SSL_RD_BUF_SIZE				2048 + 214
#define SSL_DATA_RD_QUE_SIZE		1
#define SSL_DATA_WR_QUE_SIZE		1
#define SSL_WR_STATUS_QUE_SIZE		1
#define SSL_READY_QUE_SIZE			1

xSemaphoreHandle tcp_connection_sem_start;
xQueueHandle application_wr_result;
xQueueHandle application_rd_queue;
xQueueHandle application_wr_queue;
xQueueHandle tcp_connection_ready;
//xQueueHandle dhcp_done;

typedef struct _ssl_data_pk {
	/* data buffer to read/write */
	void *buf;
	/* data length */
	uint32_t len;
	/* Returns the number of bytes read, or a negative error code */
	int32_t ret;
}ssl_data_pk_t;


typedef struct _jd_data_buf {
	/* data buffer to read/write */
	void *buf;
	/* data length */
	uint32_t len;
	/* Returns the number of bytes read, or a negative error code */
	int32_t ret;
}jd_data_buf_t;



#endif /* CONF_SERVER_H */

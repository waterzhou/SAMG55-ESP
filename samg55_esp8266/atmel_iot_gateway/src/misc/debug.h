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


#ifndef DEBUG_H_
#define DEBUG_H_

#include "IoT_SDK/config/iot_config.h"

/**
 * \name Wrappers for printing debug information
 *
 * \note The debug primitives feeds its output to printf. It is left up to
 * the real application to set up the stream.
 */
//@{

/* Debug level */
#define IoT_DBG_INFO			0x01
#define IoT_DBG_WARNING			0x02
#define IoT_DBG_SERIOUS			0x03
#define IoT_DBG_INFO_I			0x04
#define IoT_DBG_LEVEL_MASK		0x0f

/** Flag for IoT_DEBUG to enable the debug message */
#define IoT_DBG_ON            0x80U
/** Flag for IoT_DEBUG to disable the debug message */
#define IoT_DBG_OFF           0x00U

#ifdef __IoT_DEBUG__

#include "IoT_SDK/config/iot_config.h"

/* Binary semaphore handle */
extern xSemaphoreHandle xsem_dbg;

#define IoT_DEBUG(dbg_level, message) do { \
	if((((dbg_level) & IoT_DBG_LEVEL_MASK) >= IoT_DBG_MIN_LEVEL) && \
	    ((dbg_level) & IoT_DBG_ON)) { \
			if(xsem_dbg != NULL) { \
				IoT_xSemaphoreTake(xsem_dbg, portMAX_DELAY); \
				printf("(%d)(%s) ", xTaskGetTickCount(), pcTaskGetTaskName(NULL)); \
				printf message; \
				IoT_xSemaphoreGive(xsem_dbg); \
			} \
			else { \
				printf message; \
			} \
	} \
}while(0)

#else /* __IoT_DEBUG__ */

#define IoT_DEBUG(dbg_level, message)

#endif /* __IoT_DEBUG__ */

void create_dbg_sem(void);

#endif /* DEBUG_H_ */
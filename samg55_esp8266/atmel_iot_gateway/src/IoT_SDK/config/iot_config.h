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


#ifndef CONFIG_H_
#define CONFIG_H_

/* OTAU support */
//#define __IOT_OTAU__
/*
   ---------------------------------------
   ---------- Debugging options ----------
   ---------------------------------------
*/

/* Open debug information output */
#define __IoT_DEBUG__

/* Components debug switch */
#define GENERIC_DBG					IoT_DBG_ON
#define PARAM_STORE_DBG				IoT_DBG_ON
#define WIFI_DBG					IoT_DBG_ON
#define LWIP_DBG					IoT_DBG_ON
#define DOWNLOAD_DBG				IoT_DBG_ON
#define CLOUD_API_DBG				IoT_DBG_ON
#define JSON_DBG					IoT_DBG_ON
#define POLARSSL_DBG				IoT_DBG_ON
#define SSL_DBG						IoT_DBG_ON
#define SERIAL_DBG					IoT_DBG_ON
#define UPGRADE_DBG					IoT_DBG_ON
#define ZIGBEE_DBG					IoT_DBG_ON
#define IoT_DBG_MIN_LEVEL			IoT_DBG_INFO


//#define DEBUG_FLASH
/* other definitions */
#define MAX_DEVICE_SUPPORT          10
#endif /* CONFIG_H_ */
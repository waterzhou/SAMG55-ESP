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



#ifndef IOT_TEST_H_
#define IOT_TEST_H_

//#define TESTCASE_SERIAL_DOWNLOAD
//#define TESTCASE_HTTP_DOWNLOAD
//#define TESTCASE_OTA_UPGRADE
//#define TESTCASE_TCP_CLIENT
//#define TESTCASE_TCP_SERVER
//#define TESTCASE_UDP_CLIENT
//#define TESTCASE_UDP_SERVER
//#define TESTCASE_SSL_CONNECT
//#define TESTCASE_SIGNAL_STRENGTH
//#define TESTCASE_CLOUD_PUBNUB
//#define TESTCASE_AP_MODE
//#define TESTCASE_P2P_MODE
//#define TESTCASE_SMTP_CONNECT

#ifdef TESTCASE_SERIAL_DOWNLOAD
#include "serial_download/serial_download.h"
#endif

#ifdef TESTCASE_HTTP_DOWNLOAD
#include "http_download/http_download.h"
#endif

#ifdef TESTCASE_OTA_UPGRADE
#include "ota_upgrade/ota_upgrade.h"
#endif

#ifdef TESTCASE_TCP_CLIENT
#include "tcp_client/tcp_client.h"
#endif

#ifdef TESTCASE_TCP_SERVER
#include "tcp_server/tcp_server.h"
#endif

#ifdef TESTCASE_UDP_CLIENT
#include "udp_client/udp_client.h"
#endif

#ifdef TESTCASE_UDP_SERVER
#include "udp_server/udp_server.h"
#endif

#ifdef TESTCASE_SSL_CONNECT
#include "ssl_connect/ssl.h"
#endif

#ifdef TESTCASE_CLOUD_PUBNUB
#include "cloud_PubNub_demo/PubNubCloud.h"
#endif

#ifdef TESTCASE_AP_MODE
#include "ap_mode/ap.h"
#endif

#ifdef TESTCASE_P2P_MODE
#include "p2p_mode/p2p.h"
#endif

#ifdef TESTCASE_SMTP_CONNECT
#include "smtp_connect/smtp_connect.h"
#endif

#ifdef TESTCASE_SIGNAL_STRENGTH
#include "signal_strength/signal_strength.h"
#endif

#endif /* IOT_TEST_H_ */
/**
 *
 * \file
 *
 * \brief IOT spec command set.
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

#include "lan_protocol.h"
#include "config/rtos.h"
#include "misc/debug.h"
#include "misc/crc32.h"
#include "zigbee_com.h"


/*
For zigbee devices, APP sends the zigbe protocol packet by tunneling command to gateway 
who transfers it transparently to zigbee controller.
For WiFi devices, APP will communciate with it directly. The only case gateway joins is 
IFTTT feature. Gateway will send only control commands.

*/
bool check_lan_packet(uint8_t* pBuf, uint16_t len)
{
	if(pBuf[0] != SOF_INDICATER){
		IoT_DEBUG(GENERIC_DBG | IoT_DBG_WARNING, ("Invalid header\r\n"));
		return false;
	}
	
	if(pBuf[1] != len -5){
		IoT_DEBUG(GENERIC_DBG | IoT_DBG_WARNING, ("Invalid length\r\n"));
		return false;
	}
	
	//check crc32
		
	
	return true;
}
void parse_packet_prefix()
{
	lanCmdPacket_t cmdPacket;
	//memcpy(&cmdPacket, cmd_payload, LAN_CMD_PACKET_PREFIX_LEN);
	
	//cmdPacket.
	//cmdPacket.addr
	
}

void parse_iot_cmd(uint8_t cmd, uint16_t device_addr, uint8_t *cmd_payload, uint16_t payload_len)
{
	
	switch (cmd)
	{
		//network commands
		case WIFI_NODE_DISC:
			break;
		case ZIGBEE_COMMISSION:
			break;
		case SEND_NODE_INFO:
			break;
		
		//for zigbee devices
		case CMD_ZIGBEE_TUNNELING_QUERY:
		case CMD_ZIGBEE_TUNNELING_CONTROL:
		{
			Cmd_Response_t *resp;
			//send zigbee query/control command
			if(proces_zibgee_tunneling_packet(cmd_payload,payload_len)){
				if(IoT_xQueueReceive(zigbee_tunneling_queue, &resp,DEFAULT_TIMEOUT_VALUE)){
					//send to APP					
				}
				
			}
			
			break;
		}
		

		case QUERY_ALL:
		{
			
		}
		break;
		case QUERY_ATTR:
		{
			//not needed for gateway
		}
		break;
		case QUERY_CLUSTER:
		{
			//not needed for gateway
		}
		break;
		case QUERY_OTAU:
		{
			//not needed for gateway
		}
		break;
		case CONTROL_ATTR:
		{

		}
		break;
		case CONTROL_CLUSTER:
		{
			
		}
		break;
		default:
		break;
	}
}
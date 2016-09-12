/**
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

#ifndef CMDSET_H_INCLUDED
#define CMDSET_H_INCLUDED

#include "asf.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \name Command length definition. */
/* \{ */
#define SOF_LEN                1    /**< Length of command SOF string. */
#define SOF_INDICATER          0x5A /**< SOF string. */
#define CMDID_LEN              1    /**< Length of command CMDID string. */
#define CMDLNTH_LEN            1    /**< Length of command length string. */
#define CMDADDR_LEN            1    /**< Length of target address string. */
#define CMDEPPN_LEN            1    /**< Length of target endpoint/port string. */
#define CMDCHKSUM_LEN          1    /**< Length of target endpoint/port string. */
	
#define CMD_CID_LEN            2    /**< Length of cluster ID string. */
#define CMD_CINDEX_LEN         1    /**< Length of cluster index string. */
#define CMD_ATTRID_LEN         1    /**< Length of attribute ID string. */
	
#define REPORT_ATTR_HDR_LEN    (CMD_CID_LEN + CMD_CINDEX_LEN + CMD_ATTRID_LEN) /**< Length of report attribute header. */
#define REPORT_CLST_HDR_LEN    (CMD_CID_LEN + CMD_CINDEX_LEN) /**< Length of report cluster header. */
#define REPORT_ERR_HDR_LEN     1
/*@}*/

/** \name Onoff cluster length definition. */
/* \{ */
#define ONOFF_C_ATTR1_LEN      1                  /**< Length of onoff cluster attribute1. */
#define ONOFF_CLUSTER_LEN      ONOFF_C_ATTR1_LEN  /**< Length of onoff cluster. */
/*@}*/

/** \name Device temperature cluster length definition. */
/* \{ */
#define DEV_TEMP_C_ATTR1_LEN   2                  /**< Length of device temp cluster attribute1. */
#define DEV_TEMP_C_ATTR2_LEN   2                  /**< Length of device temp cluster attribute2. */
#define DEV_TEMP_C_ATTR3_LEN   2                  /**< Length of device temp cluster attribute3. */
#define DEV_TEMP_CLUSTER_LEN   (DEV_TEMP_C_ATTR1_LEN + DEV_TEMP_C_ATTR2_LEN \
								+ DEV_TEMP_C_ATTR3_LEN) /**< Length of device temp cluster. */
/*@}*/

/** \name Time calendar cluster length definition. */
/* \{ */
#define TIME_CAL_C_ATTR1_LEN   2                  /**< Length of time calendar cluster attribute1. */
#define TIME_CAL_C_ATTR2_LEN   1                  /**< Length of time calendar cluster attribute2. */
#define TIME_CAL_C_ATTR3_LEN   1                  /**< Length of time calendar cluster attribute3. */
#define TIME_CAL_C_ATTR4_LEN   4                  /**< Length of time calendar cluster attribute4. */
#define TIME_CAL_C_ATTR5_LEN   4                  /**< Length of time calendar cluster attribute5. */
#define TIME_CAL_CLUSTER_LEN   (TIME_CAL_C_ATTR1_LEN + TIME_CAL_C_ATTR2_LEN \
								+ TIME_CAL_C_ATTR3_LEN + TIME_CAL_C_ATTR4_LEN \
								+ TIME_CAL_C_ATTR5_LEN) /**< Length of device temp cluster. */
/*@}*/

/** \name Schedule cluster length definition. */
/* \{ */
#define SCHEDULE_C_ATTR1_LEN   2                  /**< Length of schedule cluster attribute1. */
#define SCHEDULE_C_ATTR2_LEN   1                  /**< Length of schedule cluster attribute2. */
#define SCHEDULE_C_ATTR3_LEN   1                  /**< Length of schedule cluster attribute3. */
#define SCHEDULE_C_ATTR4_LEN   1                  /**< Length of schedule cluster attribute4. */
#define SCHEDULE_C_ATTR5_LEN   2                  /**< Length of schedule cluster attribute5. */
#define SCHEDULE_C_ATTR6_LEN   2                  /**< Length of schedule cluster attribute5. */
#define SCHEDULE_CLUSTER_LEN   (SCHEDULE_C_ATTR1_LEN + SCHEDULE_C_ATTR2_LEN \
								+ SCHEDULE_C_ATTR3_LEN + SCHEDULE_C_ATTR4_LEN \
								+ SCHEDULE_C_ATTR5_LEN + SCHEDULE_C_ATTR6_LEN) /**< Length of schedule cluster. */
/*@}*/

/** \name Energy measurement cluster length definition. */
/* \{ */
#define ENERGY_C_ATTR1_LEN     1                  /**< Length of energy measure cluster attribute1. */
#define ENERGY_C_ATTR2_LEN     1                  /**< Length of energy measure cluster attribute2. */
#define ENERGY_C_ATTR3_LEN     2                  /**< Length of energy measure cluster attribute3. */
#define ENERGY_C_ATTR4_LEN     2                  /**< Length of energy measure cluster attribute4. */
#define ENERGY_C_ATTR5_LEN     2                  /**< Length of energy measure cluster attribute5. */
#define ENERGY_CLUSTER_LEN     (ENERGY_C_ATTR1_LEN + ENERGY_C_ATTR2_LEN \
								+ ENERGY_C_ATTR3_LEN + ENERGY_C_ATTR4_LEN \
								+ ENERGY_C_ATTR5_LEN) /**< Length of energy measure cluster. */
/*@}*/

/** \name Plug socket standard cluster length definition. */
/* \{ */
#define PLUG_TYPE_C_ATTR1_LEN  1                      /**< Length of plug socket type cluster attribute1. */
#define PLUG_TYPE_CLUSTER_LEN  PLUG_TYPE_C_ATTR1_LEN  /**< Length of plug socket type cluster. */
/*@}*/

/** \name MAC address cluster length definition. */
/* \{ */
#define MAC_ADDR_C_ATTR1_LEN   1                      /**< Length of MAC address cluster attribute1. */
#define MAC_ADDR_C_ATTR2_LEN   8                      /**< Length of MAC address cluster attribute1. */
#define MAC_ADDR_CLUSTER_LEN   (MAC_ADDR_C_ATTR1_LEN + MAC_ADDR_C_ATTR2_LEN) /**< Length of MAC address cluster. */
/*@}*/
	
/** \name Product information cluster length definition. */
/* \{ */
#define PROD_INFO_C_ATTR1_LEN  2                      /**< Length of product information cluster attribute1. */
#define PROD_INFO_C_ATTR2_LEN  2                      /**< Length of product information cluster attribute2. */
#define PROD_INFO_C_ATTR3_LEN  2                      /**< Length of product information cluster attribute3. */
#define PROD_INFO_C_ATTR4_LEN  1                      /**< Length of product information cluster attribute4. */
#define PROD_INFO_C_ATTR5_LEN  PRODUCT_SN_LEN         /**< Length of product information cluster attribute5. */
#define PROD_INFO_CLUSTER_LEN  (PROD_INFO_C_ATTR1_LEN + PROD_INFO_C_ATTR2_LEN \
								+ PROD_INFO_C_ATTR3_LEN+ PROD_INFO_C_ATTR4_LEN\
								+ PROD_INFO_C_ATTR5_LEN) /**< Length of product information cluster. */
/*@}*/

/**
 * \name Node discovery command set definition
 * IOT spec node discovery command set definition.Wi-Fi is used by default.
 * @{
 */
#define WIFI_NODE_DISC                 0x01
#define ZIGBEE_COMMISSION              0x02
#define SEND_NODE_INFO                 0x03
#define CMD_ZIGBEE_TUNNELING_QUERY	   0x13
#define CMD_ZIGBEE_TUNNELING_CONTROL   0x24

#define WIFI_NODE_DISC_RESP            0x81
#define ZIGBEE_COMMISSION_RESP         0x82
#define SEND_NODE_INFO_RESP            0x83
/*@}*/

/**
 * \name Query command set definition
 * IOT spec query command set definition.
 * @{
 */
#define QUERY_ALL                      0x10
#define QUERY_ATTR                     0x11
#define QUERY_CLUSTER                  0x12
#define QUERY_OTAU                     0x18

#define QUERY_ALL_PARAM                0xAA55
#define QUERY_OTAU_PARAM               0x4F54
/*@}*/

/**
 * \name Control command set definition
 * IOT spec control command set definition.
 * @{
 */
#define FACTORY_RESET                  0x21
#define CONTROL_ATTR                   0x22
#define CONTROL_CLUSTER                0x23
#define TUNNELING_ZIGBEE               0x24
#define OTAU_DATA                      0x28

#define FACTORY_RESET_PARAM            0xAA55
/*@}*/

/**
 * \name Report command set definition
 * IOT spec report command set definition.
 * @{
 */
#define REPORT_ALL                     0x90
#define REPORT_ATTR                    0x91
#define REPORT_CLUSTER                 0x92
#define QUERY_OTAU_RESP                0x98
#define QUERY_OTAU_DATA_RESP           0xA8
#define REPORT_ERROR                   0x9C
/*@}*/

/**
 * \name Error code definition
 * IOT spec error code definition.
 * @{
 */
#define OPT_SUCCESS                    0x00
#define OPT_FAILURE                    0x01
#define NOT_AUTHORIZED                 0x7E
#define UNSUPPORTED_CLUSTER            0x81
#define UNSUPPORTED_CMD                0x82
#define UNSUPPORTED_ATTR               0x86
#define INVALID_VALUE                  0x87
#define READ_ONLY                      0x88
#define OPT_TIMEOUT                    0x94
/*@}*/

#define CLUSTER_ONLY                   0x00
#define ATTR_INDEX1                    0x01
#define ATTR_INDEX2                    (ATTR_INDEX1 + 1)
#define ATTR_INDEX3                    (ATTR_INDEX2 + 1)
#define ATTR_INDEX4                    (ATTR_INDEX3 + 1)
#define ATTR_INDEX5                    (ATTR_INDEX4 + 1)
#define ATTR_INDEX6                    (ATTR_INDEX5 + 1)
#define ATTR_INDEX7                    (ATTR_INDEX6 + 1)
#define ATTR_INDEX8                    (ATTR_INDEX7 + 1)
#define ATTR_INDEX9                    (ATTR_INDEX8 + 1)
#define ATTR_INDEX10                   (ATTR_INDEX9 + 1)
#define ATTR_INDEX11                   (ATTR_INDEX10 + 1)

#define LAN_CMD_PACKET_PREFIX_LEN		7						   /**< Length before payload */
#define ZIGBEE_ADDRESS_RANGE_MIN		0x0000					   /**< Address range for ZIGBEE/WIFI devices */
#define ZIGBEE_ADDRESS_RANGE_MAX		0x1000
#define WIFI_ADDRESS_RANGE_MIN			0x1001
#define WIFI_ADDRESS_RANGE_MAX			0x2000

/** Operation on cluster. */
typedef struct cmd_cluster_payload {
	uint16_t          cluster_id;                                  /**< Cluster ID */
	uint8_t           cluster_index;                               /**< Cluster index */
} cmd_cluster_t;

/** Operation on attribute. */
typedef struct cmd_attr_payload {
	uint16_t          cluster_id;                                  /**< Cluster ID */
	uint8_t           cluster_index;                               /**< Cluster index */
	uint8_t           attr_id;                                     /**< Attribute ID */
} cmd_attr_t;

#pragma pack(1)
/** LAN packet protocol struct */
typedef struct lanCmdPacket{
	uint8_t sof;												   /**< Start of Frame, always 0x5A */
	uint8_t length;												   /**< packet length - sof - crc32 */
	uint8_t cmd_id;												   /**< command id */
	uint16_t addr;												   /**< target address */
	uint16_t endpoint_port;										   /**< target address extra infomation, endpoint for zigbee and port for LAN */
	void* payload;												   /**< packet payload */
	uint32_t crc32;												   /**< crc32 value */
	uint8_t payload_size;										   /**< payload size */
}lanCmdPacket_t;


#pragma pack()

#ifdef __cplusplus
}
#endif

#endif /* CMDSET_H_INCLUDED */

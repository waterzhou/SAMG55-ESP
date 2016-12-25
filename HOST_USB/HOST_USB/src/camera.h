/*
 * camera.h
 *
 * Created: 9/26/2016 4:43:16 PM
 *  Author: A41477
 */ 


#ifndef CAMERA_H_
#define CAMERA_H_

#include "conf_example.h"
#include "conf_usb_host.h"
#include <asf.h>
#if (SAMG55)
#include "flexcom.h"
#endif
#include "rtos.h"
#include "string.h"

#include "crc8.h"

// 2016/11 PA 
#define DATA_BLOCK   (4*1024)

/*typedef struct PACK{
	union PACK{
		struct PACK{
			uint8_t header;
			uint8_t type;
			union PACK{
				struct PACK{
					uint8_t ack;
					uint8_t checksum;
				}snapshotRsp;
				struct PACK{
					uint16_t sequence;
					uint16_t dataLength;
					uint8_t picture_buff[DATA_BLOCK+1];//with checksum
				}blockSeqRsp;
			};
		};
		uint8_t payload[7+DATA_BLOCK];
	};
	uint8_t blockStatus;  //available indicator for block data
}PictureDataRsp_t;*/


typedef struct VALIDDATA{
	uint16_t dataLength;
	uint8_t picture_buff[DATA_BLOCK+1];//with checksum
}BlockSeqData;

#define COM_HEAD			0x8E
#define COM_CMD_TYPE_CMD	0x00
#define COM_CMD_TYPE_ACK	0x01
#define COM_CMD_TYPE_DATA	0x02

#define CMD_GET_SNAPSHOT_INIT    0xFF
#define CMD_GET_SNAPSHOT    0x00
#define CMD_GET_BLOCK_DATA	0x01

typedef enum{
	CAMERA_STATE_INIT = 0,
	CAMERA_STATE_GET_SNAPSHOT,
	CAMERA_STATE_READ_DATA,
	CAMERA_STATE_REST,
}CameraState_t;


typedef enum{
	PICTURE_DATA_READY = 0,
	PICTURE_DATA_PROCESSING,
	PICTURE_DATA_OBSOLATE,
}PictureDataStatus_t;

void CameraPictureResetStateMachine(void);
void CameraPictureDataHandler(uint8_t data);
bool CameraPictureSnapshotReq(uint8_t command);
// 2016/11 PA 
bool CameraPictureBlockReq(uint16_t blockSeq);
void CameraPictureRxNotify(void);
void taskCamera( void *pvParameters);
#endif /* CAMERA_H_ */
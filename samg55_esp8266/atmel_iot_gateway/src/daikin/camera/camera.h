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


#define COM_HEAD			0x8E
#define COM_CMD_TYPE_CMD	0x00
#define COM_CMD_TYPE_ACK	0x01
#define COM_CMD_TYPE_DATA	0x02

#define CMD_GET_SNAPSHOT    0x00
#define CMD_GET_BLOCK_DATA	0x01




typedef enum{
	PICTURE_DATA_READY = 0,
	PICTURE_DATA_PROCESSING,
	PICTURE_DATA_OBSOLATE,
}PictureDataStatus_t;


void CameraPictureDataHandler();
bool CameraPictureSnapshotReq(uint8_t command);
bool CameraPictureBlockReq(uint8_t blockSeq);
void CameraPictureRxNotify(void);
void camera_task( void *pvParameters);
#endif /* CAMERA_H_ */
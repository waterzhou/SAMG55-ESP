/*
 * temperature.h
 *
 * Created: 9/26/2016 4:43:43 PM
 *  Author: A41477
 */ 


#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_

#include <asf.h>
#if (SAMG55)
#include "flexcom.h"
#endif
#include "daikin/config/rtos.h"
#include "string.h"

//Serial port Settings
#define SERIAL_FRAME_INTERVAL	10 //10ms

#define USART_BAUDRATE					195312UL//115200UL
#define USART_PARITY					US_MR_PAR_EVEN//US_MR_PAR_NO
#define USART_CHRL						US_MR_CHRL_8_BIT
#define USART_NBSTOP					US_MR_NBSTOP_1_BIT

#define  BOARD_ZIGBEE_GATEWAY 1

/* Serial port configuration */
#define TSENSOR_SERIAL_PORT					USART3
#define TSENSOR_SERIAL_PORT_FLEXCOM_ID		ID_FLEXCOM3
#define TSENSOR_SERIAL_PORT_FLEXCOM			FLEXCOM3
#define TSENSOR_SERIAL_PORT_IRQn			FLEXCOM3_IRQn
#define tSensor_uart_isr_handler			FLEXCOM3_Handler

#define SERIAL_PORT_INT_PRIO	14
#define SERIAL_TX_COMPLETED		1

typedef enum{
	MEASUREMENT_INIT				= 0x00,
	MEASUREMENT_START				= 0x01,
	MEASUREMENT_INIT_QUERY_AIR_INFO	= 0x02,
	MEASUREMENT_QUERY_AIR_INFO		= 0x03,
	MEASUREMENT_STOP				= 0x04,
	MEASUREMENT_WAIT				= 0x05,
}TsensorDataQueryState_t;

//Const for COMM
#define G55_TO_THERMAL_SYMBOL   0xAA
#define THERMAL_TO_G55_SYMBOL   0xCC
enum{
	INIT_SENSATION_MEASUREMENT  = 0x01,
	SENSATION_MEASUREMENT_START = 0x02,
	SENSATION_MEASUREMENT_STOP  = 0x03
};

#define MODE_THERMAL_MEARUREMENT			0x10
#define MODE_AIR_CONDITION_INFO				0x88
#define MODE_GRID_EYE_OUTPUT				0x33
#define MODE_CORRECTION_DATA_IMAGE			0x99
#define MODE_HIGH_RESULATION_THERMAL_IMAGE	0x80
#define MODE_HUMAN_DETECTION_RESULT			0x9E
#define MODE_COMMUNICATION_ERROR			0xE1

#define TSENSOR_RECV_BUFF_SIZE  5000

typedef struct{
	uint16_t len;
	uint8_t  payload[TSENSOR_RECV_BUFF_SIZE];
}uart_buff_t;

#define HUMAN_INFO_MAX  10

typedef struct{
	uint8_t pixel[64];
}GridEyeThermalImage_t;

typedef struct{
	uint8_t correctionDateImage[128];
}CorrectionDateImage_t;

typedef struct{
	uint8_t pixel[4416];
}HighResolutionThermalImage_t;

typedef struct{
	int8_t winter_metabolism;
	int8_t summer_metabolism;
	uint8_t position_Horizontal_angle;
	uint8_t position_Vertical_angle_head;
	uint8_t position_Vertical_angle_foot;
	uint8_t position_Distance;
	uint16_t reserved;
}HumanDetectionInfo_t;

typedef struct{
	uint8_t errorCode;
	uint8_t info;
}CommunicationError_t;


typedef struct{
	uint8_t mode;
	uint16_t size;
	GridEyeThermalImage_t			gridEyeThermalImage;
	CorrectionDateImage_t			correctionDateImage;
	HighResolutionThermalImage_t	highResolutionThermalImage;
	HumanDetectionInfo_t			humanDetectionInfo[HUMAN_INFO_MAX];
	CommunicationError_t			communicationError;
	uint8_t payload[4416];
}ThermalParam_t;

void tSensor_serial_init(void);
void resetTsensorCommStateMachine(void);
void tSensor_handler(void);
void sendToTsensorUart(uint8_t *buff, int32_t buffSize);

void Temp_Measure_Command_Send(uint8_t command);
void Temp_Measure_Get_Air_Condition_Info(uint8_t roomTemperature, uint8_t roomHumidity);
void sensor_task( void *pvParameters);

#endif /* TEMPERATURE_H_ */
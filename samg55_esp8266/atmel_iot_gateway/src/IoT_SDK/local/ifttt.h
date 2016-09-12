/*
 * ifttt.h
 *
 * Created: 2015-06-26 15:31:31
 *  Author: matt.qian
 */ 


#ifndef IFTTT_H_
#define IFTTT_H_

enum{
	CONDITION_GT,		// ">"
	CONDITION_GE,		// ">="
	CONDITION_LT,		// "<"
	CONDITION_LE,		// "<="
	CONDITION_EQ,		// "="
	CONDITION_NQ		// "!="
	};

#pragma pack(1)
typedef struct ifttt_config{
	uint16_t srcAddr;			//source device address
	uint16_t srcClusterID;		//source device cluster id
	uint16_t srcAttributeID;	//source device attribute id
	uint8_t condition;			//trigger condition
	union{						//source value
		uint8_t srcValue8;
		uint16_t scrValue16;
		uint32_t scrValue32;
		};
		
	uint16_t dstAddr;			//target device address
	uint16_t dstClusterID;		//target device cluster id
	uint16_t dstAttributeID;	//target device attribute id
	union{						//target device value to be applied
		uint8_t dstValue8;
		uint16_t dstValue16;
		uint32_t dstValue32;
	};
	
}ifttt_config_t;
#pragma pack()


#endif /* IFTTT_H_ */
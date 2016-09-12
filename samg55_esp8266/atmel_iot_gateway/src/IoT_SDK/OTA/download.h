/*
 * download.h
 *
 * Created: 3/10/2015 2:27:46 PM
 *  Author: water.zhou
 */ 


#ifndef DOWNLOAD_H_
#define DOWNLOAD_H_

/* Serial port timer configuration */
#define SERIAL_TMR_PERIOD	1 //Tick in timer period, 10ms
#define SERIAL_TMR_RELOAD	0

#define MAXIMUM_DATA_LENGTH		512

#define SERIAL_IN_QUEUE_LEN			2
#define SERIAL_OUT_QUEUE_LEN		1
#define SERIAL_OUT_RET_QUEUE_LEN	1

#define SERIAL_PORT_INT_PRIO	14

#define SERIAL_FRAME_INTERVAL	10 //10ms

#define SERIAL_TX_COMPLETED		1

#define SERIAL_SEND_TIMEOUT     200
#define SERIAL_RECV_TIMEOUT     500

typedef struct _serial_in_pk {
	/* data buffer to Ethernet */
	uint8_t buf[MAXIMUM_DATA_LENGTH];
	/* data length */
	uint32_t len;
}serial_in_pk_t;

typedef struct _serial_out_pk {
	/* data buffer to Serial port */
	uint8_t *buf;
	/* data length */
	uint32_t len;
}serial_out_pk_t;

extern xQueueHandle serial_in_queue;
extern xQueueHandle serial_out_queue;
extern xQueueHandle serial_out_ret_queue;
void serial_out(void *parameter);
/** Usart download read task */
void usart_download_rd_tsk(void *pvParameters);
/** Usart download write task */
void usart_download_wr_tsk(void *pvParameters);
uint8_t* receiveResponse(uint8_t* len);
void usarttest();

#endif /* INCFILE1_H_ */
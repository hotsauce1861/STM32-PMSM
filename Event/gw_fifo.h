#ifndef GW_FIFO_H
#define GW_FIFO_H
#include "stdint.h"
#include "gw_event.h"

struct gw_fifo{

#define	FIFO_SIZE	60

	struct 	gw_event **event_table;	//指向gw_event队列
	uint8_t front;	//队首
	uint8_t rear;	//队尾

};

#endif


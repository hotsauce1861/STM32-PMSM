#ifndef GW_MSGS_H
#define GW_MSGS_H

#define MSGS_FIFO_SIZE 10
#include "gw_type.h"
struct gw_msg_mod{
	uint8_t task_id;
	void* pstr;
};
typedef struct gw_msg_mod gw_msg_mod_typedef;

struct gw_msg_fifo_mod{
	uint8_t size;
	gw_msg_mod_typedef msgs[MSGS_FIFO_SIZE];
	uint8_t front;
	uint8_t rear;
};

typedef struct gw_msg_fifo_mod gw_msg_fifo_mod_typedef;

void gw_msg_fifo_clear(gw_msg_fifo_mod_typedef * const pfifo);
GW_RESULT gw_msg_fifo_is_full(gw_msg_fifo_mod_typedef* const pfifo);
GW_RESULT gw_msg_fifo_is_empty(gw_msg_fifo_mod_typedef* const pfifo);
GW_RESULT gw_msg_fifo_enqueue(gw_msg_fifo_mod_typedef* const pfifo, 
								gw_msg_mod_typedef* const pmsg);

GW_RESULT gw_msg_fifo_dequeue(gw_msg_fifo_mod_typedef* const pfifo,
								gw_msg_mod_typedef* const pmsg);

#endif

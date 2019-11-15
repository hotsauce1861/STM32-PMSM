#ifndef GW_FIFO_H
#define GW_FIFO_H
#include "stdint.h"
#include "gw_event.h"

#define GW_FIFO_SIZE 	20

struct gw_table{
	const uint8_t size;
	struct gw_event event_table[GW_FIFO_SIZE];
	uint8_t rear;
	uint8_t front;
};

typedef struct gw_fifo gw_fifo_typedef;

void gw_event_fifo_init(void);
void gw_reset_event_status(void);
void gw_poll_event_task(void);
void gw_execute_event_task(void);
void gw_global_timer_add(void);
__weak void task_init(void);

void gw_list_add_event(struct gw_event envent);


#endif


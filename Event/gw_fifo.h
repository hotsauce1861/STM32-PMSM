#ifndef GW_FIFO_H
#define GW_FIFO_H
#include "stdint.h"
#include "gw_event.h"

void gw_event_fifo_init(void);
void gw_reset_event_status(void);
void gw_poll_event_task(void);
void gw_execute_event_task(void);
void gw_global_timer_add(void);
__weak void task_init(void);

uint8_t gw_is_event_empty(void);


#endif


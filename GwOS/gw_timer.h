#ifndef GW_TIMER_H
#define GW_TIMER_H
#include <stdint.h>
#include "stm32f10x.h"

/**
 * timebase-1ms 进行一次累加
 */
struct gw_timer{
	uint32_t	timestamp;	
};


extern struct gw_timer global_timer;

void timer_base_config(void);
void timer_reset(struct gw_timer *timer);
void timer_add(struct gw_timer *timer);
uint32_t get_timer_stamp(struct gw_timer *timer);
void set_timer_value(struct gw_timer *timer, uint32_t value);

#endif


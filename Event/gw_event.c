#include "gw_event.h"

#define GLOBAL_TIMER global_timer	

void gw_event_init(struct gw_event *event){
	event->status = DISABLE;
	event->timer = &GLOBAL_TIMER;
}

static void set_poll_time(void){
	
}

void update_poll_time(void){
	
}


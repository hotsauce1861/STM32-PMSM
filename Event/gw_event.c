#include "gw_event.h"


#define GLOBAL_TIMER global_timer	

#define EVENT_MAX	30
/*
	const char* name;				//事件名称
	uint8_t type;					//事件类型
	uint8_t status; 				//事件状态
	uint32_t timestamp; 			//时间戳
	void* (CALLBACK)(void*) task;	//回调函数
	static struct gw_timer* timer;	//全局定时器	
*/


void gw_event_init(struct gw_event *event){
	event->status = DISABLE;
	event->g_timer = &GLOBAL_TIMER;
}

void set_poll_time(void){
	
}

void update_poll_time(void){
	
}


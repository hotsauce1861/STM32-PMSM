#include <stdio.h>

#include "gw_fifo.h"
#include "gw_event.h"
#include "gw_timer.h"
#include "gw_tasks.h"

#define EVENT_MAX	30
/*
	const char* name;				//事件名称
	uint8_t type;					//事件类型
	uint8_t status; 				//事件状态
	uint32_t timestamp; 			//时间戳
	void* (CALLBACK)(void*) task;	//回调函数
	static struct gw_timer* timer;	//全局定时器	
*/
#define EVENT_SIZE 3
static struct gw_event event_table[] = 
{
	{	
		.name = "event1",
		.type = TYPE_IRQ, 
		.status = DISABLE,
		.task = task_svpwm,
		.poll_time = 10,
		.g_timer = &global_timer,		
	},
	{
		.name = "event2",
		.type = TYPE_IRQ, 
		.status = DISABLE,
		.task = task_idle,
		.poll_time = 100,
		.g_timer = &global_timer,
	},	
	
	{
		.name = "get_rpm",
		.type = TYPE_IRQ, 
		.status = DISABLE,
		.task = task_get_rpm,
		.poll_time = 1,
		.g_timer = &global_timer,
	},		
};


//#define EVENT_SIZE gw_get_event_list_size()

uint8_t gw_get_event_list_size(void){
	return sizeof(event_table)/sizeof(struct gw_event);
}

uint8_t gw_is_event_empty(void){
	if(gw_get_event_list_size() == 0){
		return 1;
	}
	return 0;
}

void gw_event_fifo_init(void){

	uint8_t i = 0;
	uint8_t size = EVENT_SIZE;
	struct gw_event *p_event;
	for(; i<size; i++){
		p_event = &event_table[i%size];
		p_event->status = DISABLE;
		p_event->timestamp = 0;
	}
	global_timer.timestamp = 0;
}

void gw_reset_event_status(void){
	uint8_t i = 0;
	uint8_t size = EVENT_SIZE;	
	for(; i<size; i++){
		if(event_table[i%size].status == ENABLE){
			event_table[i%size].status = DISABLE;			
		}
	}
}

void gw_poll_event_task(void){

	uint8_t i = 0;
	uint8_t size = EVENT_SIZE;
	struct gw_event *p_event;
	for(; i<size; i++){
		p_event = &event_table[i%size];
		if( p_event->poll_time + p_event->timestamp < p_event->g_timer->timestamp){
			p_event->timestamp = p_event->g_timer->timestamp;
			p_event->status = ENABLE;
		}
	}
}

void gw_execute_event_task(void){
	uint8_t i = 0;
	uint8_t size = EVENT_SIZE;
	struct gw_event *p_event;
	for(; i<size; i++){
		p_event = &event_table[i%size];
		if(p_event->status == ENABLE && p_event->task != NULL){
			p_event->task();
			p_event->status = DISABLE;
		}
	}
}

void gw_global_timer_add(void){
	timer_add(&global_timer);
}

uint8_t gw_list_is_empty(){
	return 0;
}

void gw_list_add_event(struct gw_event envent){

}

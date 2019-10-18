#ifndef GW_EVENT_H
#define GW_EVENT_H
#include <stdint.h>

#include "gw_timer.h"

#define GW_ENABLE 	1
#define GW_DISABLE 	0

#define TYPE_IRQ	0x01
#define TYPE_POLL	0x02

struct gw_event {
	const char* name;				//事件名称
	uint8_t type;					//事件类型
	uint8_t status;					//事件状态
	uint32_t timestamp;				//时间戳
	uint32_t poll_time;				//轮询执行时间
	void (*task)(void);	//回调函数
	struct gw_timer* g_timer;	//全局定时器	
};

#endif

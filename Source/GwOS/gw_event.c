/**
************************************************************************************************
 * @file    : gw_event.c
 * @brief   : Interface to manage events
 * @details : None
 * @date    : 11-09-2018
 * @version : v1.0.0.0
 * @author  : UncleMac
 * @email   : zhaojunhui1861@163.com
 *
 *      @license    : GNU General Public License, version 3 (GPL-3.0)
 *
 *      Copyright (C)2019 UncleMac.
 *
 *      This code is open source: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *
 *      This code is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program.If not, see < https://www.gnu.org/licenses/>.
*************************************************************************************************
*/

#include "gw_event.h"
#include "gw_type.h"

#define GLOBAL_TIMER global_timer	

/*
struct gw_event {
    const char* name;				//事件名称
    uint32_t id;					//时间ID
    uint8_t type;					//事件类型
    uint8_t status;					//事件状态
    uint32_t priority;              //事件优先级
    uint32_t timestamp;				//时间戳
    uint32_t poll_time;				//轮询执行时间
    gw_msg_t msg;                   //消息
    p_init_func init_task;          //任务初始化函数
    p_exec_func exec_task;          //轮询任务回调函数
    p_exec_args_func exec_args_task;//带参数的回调函数
    struct gw_timer* g_timer;		//全局定时器
};
*/

#define GW_DEFAULT_EV_ID                0
#define GW_DEFAULT_EV_PRIORITY          0
#define GW_DEFAULT_EV_POLLTIME          5
#define GW_DEFAULT_EV_TYPE              TYPE_POLL
#define GW_DEFAULT_EV_STATUS            DISABLE
#define GW_DEFAULT_EV_INIT_FUNC         NULL
#define GW_DEFAULT_EV_EXEC_FUNC         NULL
#define GW_DEFAULT_EV_EXEC_ARGS_FUNC    NULL

void gw_event_init_default(gw_event_t *pev, const char* name, DATA_TYPE id){
    pev->name = name;
    gw_event_set_id(pev, id);
    gw_event_set_type(pev, TYPE_POLL);
    gw_event_set_priority(pev, GW_DEFAULT_EV_PRIORITY);
    gw_event_set_poll_time(pev, GW_DEFAULT_EV_POLLTIME);
    gw_event_set_init_func(pev, GW_DEFAULT_EV_INIT_FUNC);
    gw_event_set_exec_task(pev, GW_DEFAULT_EV_EXEC_FUNC);
    gw_event_set_exec_args_task(pev, GW_DEFAULT_EV_EXEC_ARGS_FUNC);
    gw_event_set_timer(pev, &GLOBAL_TIMER);
    gw_event_set_timestamp(pev,GLOBAL_TIMER.timestamp);
    gw_msg_init(&pev->msg,id);
}

void gw_event_init(gw_event_t *pev, const char* name, DATA_TYPE id){
    pev->name = name;
    gw_event_set_id(pev, id);
    gw_event_set_type(pev, TYPE_POLL);
    gw_event_set_priority(pev, GW_DEFAULT_EV_PRIORITY);
    gw_event_set_poll_time(pev, GW_DEFAULT_EV_POLLTIME);
    gw_event_set_init_func(pev, GW_DEFAULT_EV_INIT_FUNC);
    gw_event_set_exec_task(pev, GW_DEFAULT_EV_EXEC_FUNC);
    gw_event_set_exec_args_task(pev, GW_DEFAULT_EV_EXEC_ARGS_FUNC);
    gw_event_set_timer(pev, &GLOBAL_TIMER);
    gw_event_set_timestamp(pev,GLOBAL_TIMER.timestamp);
    gw_msg_init(&pev->msg,id);
}

void gw_event_set_timer(gw_event_t *pev, struct gw_timer* ptimer){
    pev->g_timer = ptimer;
}

void gw_event_set_timestamp(gw_event_t *pev, uint32_t timestamp){
    pev->timestamp = timestamp;
}

void gw_event_set_id(gw_event_t *pev, DATA_TYPE id){
    pev->id = id;
}

void gw_event_set_type(gw_event_t *pev, uint8_t type){
    pev->type = type;
}

void gw_event_set_msg(gw_event_t *pev, const gw_msg_t msg){
    pev->msg = msg;
}

void gw_event_set_status(gw_event_t *pev, uint8_t status){
    pev->status = status;
}

void gw_event_set_init_func(gw_event_t *pev, p_init_func init_task){
    pev->init_task = init_task;
}

void gw_event_set_exec_task(gw_event_t *pev, p_exec_func exec_task){
    pev->exec_task = exec_task;
}

void gw_event_set_exec_args_task(gw_event_t *pev, p_exec_args_func exec_args_task){
    pev->exec_args_task = exec_args_task;
}

void gw_event_set_poll_time(gw_event_t *pev, uint32_t poll_time){
    pev->poll_time = poll_time;
}

void gw_event_set_priority(gw_event_t *pev, uint32_t priority){
    pev->priority = priority;
}

DATA_TYPE gw_event_get_id(gw_event_t *pev){
    return pev->id;
}

uint8_t gw_event_get_type(gw_event_t *pev){
    return pev->type;
}

gw_msg_t gw_event_get_msg(gw_event_t *pev){
    return pev->msg;
}

uint8_t gw_event_get_status(gw_event_t *pev){
    return pev->status;
}

uint32_t gw_event_get_poll_time(gw_event_t *pev){
    return pev->poll_time;
}

uint32_t gw_event_get_priority(gw_event_t *pev){
    return pev->priority;
}

p_init_func gw_event_get_init_func(gw_event_t *pev){
    return pev->init_task;
}

p_exec_func gw_event_get_exec_task(gw_event_t *pev){
    return pev->exec_task;
}

p_exec_args_func gw_event_get_exec_args_task(gw_event_t *pev){
    return pev->exec_args_task;
}

struct gw_timer* gw_event_get_timer(gw_event_t *pev){
    return pev->g_timer;
}


/**
************************************************************************************************
 * @file    : gw_fifo.c
 * @brief   : Event container management API
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

#include <stdio.h>
#include "gw_fifo.h"
#include "gw_event.h"
#include "gw_timer.h"
#include "gw_tasks.h"
#include "gw_type.h"

#define EVENT_MAX	3
//extern struct gw_event event_table[EVENT_SIZE];
struct gw_event event_table[EVENT_SIZE];
#if 0
static struct gw_event event_table[EVENT_SIZE] =
{

    {
        .name = "event1",
        .type = TYPE_IRQ,
        .status = ENABLE,
        .exec_task = task_svpwm,
        .poll_time = 20,
        .g_timer = &global_timer,
    },

    {
        .name = "event2",
        .type = TYPE_IRQ,
        .status = DISABLE,
        .exec_task = task_idle,
        .poll_time = 10,
        .g_timer = &global_timer,
    },

    {
        .name = "get_rpm",
        .type = TYPE_IRQ,
        .status = DISABLE,
        .exec_task = task_get_rpm,
        .poll_time = 10,
        .g_timer = &global_timer,
    },

};
#endif
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
    GW_ENTER_CRITICAL_AREA;
    for(; i<size; i++){
        p_event = &event_table[i%size];
        if( p_event->poll_time + p_event->timestamp < p_event->g_timer->timestamp){
            p_event->timestamp = p_event->g_timer->timestamp;
            p_event->status = ENABLE;
        }
    }
    GW_EXIT_CRITICAL_AREA;
}

void gw_execute_event_task(void){
    uint8_t i = 0;
    uint8_t size = EVENT_SIZE;
    struct gw_event *p_event;
    GW_ENTER_CRITICAL_AREA;
    for(; i<size; i++){
        p_event = &event_table[i%size];
        if(p_event->status == ENABLE && p_event->exec_task != NULL){
            p_event->exec_task();
            p_event->status = DISABLE;
        }
    }
    GW_EXIT_CRITICAL_AREA;
}

void gw_global_timer_add(void){
    timer_add(&global_timer);
}

uint8_t gw_fifo_is_empty(gw_fifo_typedef *pfifo){

    if((pfifo->rear + 1) % GW_FIFO_SIZE == pfifo->front){
        return GW_TRUE;
    }
    return GW_FALSE;
}

uint8_t gw_fifo_is_full(gw_fifo_typedef *pfifo){

    if(pfifo->rear == pfifo->front){
        return GW_TRUE;
    }
    return GW_FALSE;
}


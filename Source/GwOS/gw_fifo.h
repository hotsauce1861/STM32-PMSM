/**
************************************************************************************************
 * @file    : gw_fifo.h
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

#ifndef GW_FIFO_H
#define GW_FIFO_H
#include "stdint.h"
#include "gw_event.h"

#define GW_FIFO_SIZE 	20
#define EVENT_SIZE      3

struct gw_table{
	const uint8_t size;
	struct gw_event event_table[GW_FIFO_SIZE];
	uint8_t rear;
	uint8_t front;
};

typedef struct gw_table gw_fifo_typedef;

void gw_event_fifo_init(void);
void gw_reset_event_status(void);
void gw_poll_event_task(void);
void gw_execute_event_task(void);
void gw_global_timer_add(void);
//__weak void task_init(void);

void gw_list_add_event(struct gw_event envent);

#endif


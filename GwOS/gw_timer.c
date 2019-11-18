/**
************************************************************************************************
 * @file    : gw_timer.c
 * @brief   : Timer management API
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
#include "gw_timer.h"

struct gw_timer global_timer;

static void ll_time_init(void){
	hal_system_timer_config();
}

void timer_base_init(void){
	ll_time_init();
}

void timer_reset(struct gw_timer *timer){
	timer->timestamp = 0;
}

void timer_add(struct gw_timer *timer){
	timer->timestamp++;
}

uint32_t get_timer_stamp(struct gw_timer *timer){
	return timer->timestamp;
}

void set_timer_value(struct gw_timer *timer, uint32_t value){
	timer->timestamp = value;
}

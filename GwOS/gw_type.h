/**
************************************************************************************************
 * @file    : gw_type.h
 * @brief   :
 * @details :
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

#ifndef GW_TYPE_H
#define GW_TYPE_H
#include <stdint.h>
#include <stdlib.h>

#include "gw_hal.h"

#define GW_DISABLE       0
#define GW_ENABLE        1

#define GW_RESULT		int8_t

#define GW_ERROR		-1
#define GW_ERROR_TASK	-2

#define GW_SUCCESS		 1

#define GW_TRUE			 1
#define GW_FALSE		 0

#define TYPE_IRQ	0x01
#define TYPE_POLL	0x02

#define DATA_TYPE       uint32_t

#define GW_ENABLE_INTERRUPTS	HAL_ENABLE_IRQ
#define GW_DISABLE_INTERRUPTS	HAL_DISABLE_IRQ

#define GW_ENTER_CRITICAL_AREA	HAL_DISABLE_IRQ
#define GW_EXIT_CRITICAL_AREA	HAL_ENABLE_IRQ

//#define GW_MALLOC((x))			malloc((x))
//#define GW_FREE((x))			free((x))

#define GW_MALLOC(x)			malloc(x)
#define GW_FREE(x)      		free(x)

#endif

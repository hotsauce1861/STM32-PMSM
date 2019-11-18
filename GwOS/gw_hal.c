/**
************************************************************************************************
 * @file    : gw_msgs.c
 * @brief   :
 * @details :
 * @date    : 11-09-2019
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
#include "stm32f10x.h"
#include "core_cm3.h"
#include "system_stm32f10x.h"
#include "gw_hal.h"

volatile uint32_t time_cnt = 0;

void gw_hal_delay(volatile uint32_t ntime){

	time_cnt = ntime;
	while(time_cnt != 0);
	
}

void gw_hal_dec(void){
	if (time_cnt != 0x00)
	{ 
		time_cnt--;
	}	
}


void hal_system_timer_config(void){
		/* Setup SysTick Timer for 1 msec interrupts.
	   ------------------------------------------
	  1. The SysTick_Config() function is a CMSIS function which configure:
		 - The SysTick Reload register with value passed as function parameter.
		 - Configure the SysTick IRQ priority to the lowest value (0x0F).
		 - Reset the SysTick Counter register.
		 - Configure the SysTick Counter clock source to be Core Clock Source (HCLK).
		 - Enable the SysTick Interrupt.
		 - Start the SysTick Counter.
	  
	  2. You can change the SysTick Clock source to be HCLK_Div8 by calling the
		 SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8) just after the
		 SysTick_Config() function call. The SysTick_CLKSourceConfig() is defined
		 inside the misc.c file.
	
	  3. You can change the SysTick IRQ priority by calling the
		 NVIC_SetPriority(SysTick_IRQn,...) just after the SysTick_Config() function 
		 call. The NVIC_SetPriority() is defined inside the core_cm3.h file.
	
	  4. To adjust the SysTick time base, use the following formula:
							  
		   Reload Value = SysTick Counter Clock (Hz) x	Desired Time base (s)
	  
		 - Reload Value is the parameter to be passed for SysTick_Config() function
		 - Reload Value should not exceed 0xFFFFFF
	 */
	if (SysTick_Config(SystemCoreClock / 1000))
	{ 
	  /* Capture error */ 
		while (1);
    }
}

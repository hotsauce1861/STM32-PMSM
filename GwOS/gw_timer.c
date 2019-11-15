#include "gw_timer.h"

struct gw_timer global_timer;

static void ll_time_init(void){
	
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

void timer_base_config(void){
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

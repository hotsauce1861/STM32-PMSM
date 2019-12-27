#include <stdio.h>

#include "svpwm_module.h"
#include "svpwm_math.h"
#include "usart_driver.h"
#include "task_display.h"
#include "task_motor_control.h"
#include "task_key.h"
#include "task_main.h"
#include "user_config.h"
//gw_os include
#include "gw_timer.h"
#include "gw_fifo.h"
#include "gw_log.h"
#include "gw_tasks.h"
#include "gw_event.h"
#include "gw_fifo.h"

//bsp include
#include "usart_driver.h"
#include "position.h"
#include "motor_protocol.h"

extern void task_key_create(void);
extern void task_display_create(void);
extern void task_motor_control_create(void);

void task_bsp_init(void){
	/* init usart*/
	usart_init();
	usart_set_rx_cbk(&user_uart_mod, motor_get_cmd_from_uart,&user_uart_mod);	
}

void task_system_init(void){
	timer_base_init();
	// gw_os init
	gw_task_list_init();
	gw_msg_fifo_init(&msg_fifo);	
}

void task_create(void){
		
	//task_key_create();	
	//task_display_create();
	task_motor_control_create();
}

void task_main(void){
	/**
		gw os init here
	*/
#if USE_TASK_LIST
	gw_task_init_process();
#else
	gw_event_fifo_init();
#endif

	while (1)
	{				
#if USE_TASK_LIST	
		gw_task_process();
#else
		gw_execute_event_task();
#endif
	}
}

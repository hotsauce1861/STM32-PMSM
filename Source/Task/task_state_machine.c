#include <stdio.h>
#include <stdint.h>
#include "task_state_machine.h"

#include "stm32f10x.h"
#include "user_config.h"
#include "gw_tasks.h"
#include "user_state_machine.h"


void task_state_machine_init(void);
void task_state_machine(void* args);

void task_state_machine_create(){
	gw_task_args_create("state_machine", TASK_ID_STATE_MACHINE, 
					TYPE_POLL, GW_DISABLE, 10,
					task_state_machine_init,
					task_state_machine);
}
/**
 * @brief Initialize motor control task 
 */
void task_state_machine_init(void){
	
	
}

void task_state_machine(void* args){
    struct gw_event *pev = (struct gw_event *)args;
	gw_msg_fifo_t * const pmsg_fifo = &msg_fifo;
    gw_msg_t msg;
    if(gw_msg_receive(pev->id, pmsg_fifo, &msg) == GW_TRUE){
        /**
            TODO
        */               
        printf("%016d: name:[%s] id:[%d] msg:[%s]\n", pev->g_timer->timestamp,
               pev->name, pev->id,(uint8_t*)msg.pstr);
    }
	
    pev->msg.pstr = "MSG from task_04: Hello I am task 04";
    if(gw_msg_send_msg(TASK_ID_DISPLAY, pmsg_fifo, &pev->msg) == GW_TRUE){
        /**
            TODO
        */
    }
}

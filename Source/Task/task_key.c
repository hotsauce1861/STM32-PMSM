#include "task_key.h"
#include "user_config.h"
#include "gw_tasks.h"

void task_key_init(void);
void task_key(void* args);

void task_key_create(void){
	gw_task_args_create("display", TASK_ID_KEY, 
				TYPE_POLL, GW_DISABLE, 5,
				task_key_init,
				task_key);
}

void task_key_init(void){
	
}

void task_key(void* args){
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

    if(gw_msg_send_msg(TASK_ID_MOTOR, pmsg_fifo, &pev->msg) == GW_TRUE){
        /**
            TODO
        */
    }

    if(gw_msg_send_msg(TASK_ID_DISPLAY, pmsg_fifo, &pev->msg) == GW_TRUE){
        /**
            TODO
        */
    }

}


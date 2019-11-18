#include "task_display.h"
#include "user_config.h"
#include "gw_tasks.h"

void task_display_init(void);
void task_display(void* args);

void task_display_create(void){
	gw_task_args_create("display", TASK_ID_DISPLAY, 
				TYPE_POLL, GW_DISABLE, 5,
				task_display_init,
				task_display);
}

void task_display_init(void){
	
}

void task_display(void* args){

}



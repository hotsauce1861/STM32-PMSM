/**
************************************************************************************************
 * @file    : gw_task.c
 * @brief   : Task management API
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
#include "gw_tasks.h"
#include "gw_list.h"
#include "gw_timer.h"
#include "gw_log.h"
static gw_list* ptask_list = NULL;

/**
 * @brief Initialize the task list
 */
void gw_task_list_init(void){
    ptask_list = gw_list_init();
    ptask_list->event.g_timer = &global_timer;
}

/**
 * @brief create a task
 * @param name      - pointer to ths string of task name
 * @param taskid    - task id
 * @param type      - task type, see gw_type.h
 * @param status    - task status, see gw_type.h
 * @param poll_time - time circle to schedule task
 * @param priority  - priority of task, low value is high priority
 * @param init_func - callback when initialization
 * @param p_exec_func - callback without args
 * @param p_exec_args_func - callback with args
 */
void gw_task_create(const char *name,DATA_TYPE taskid,uint8_t type,
                    uint8_t status, uint32_t poll_time,uint32_t priority,
                    p_init_func init_func,
                    p_exec_func exec_task,
                    p_exec_args_func exec_args_task){

    DATA_TYPE task_id = taskid;
    if(task_id == 0){
        task_id = global_timer.timestamp + ptask_list->data;
    }
    gw_list_node *pnode = gw_list_node_init(task_id);

    gw_event_init(&pnode->event,name, task_id);

    gw_event_set_type(&pnode->event, type);
    gw_event_set_status(&pnode->event, status);
    gw_event_set_priority(&pnode->event, priority);
    gw_event_set_poll_time(&pnode->event, poll_time);
    gw_event_set_init_func(&pnode->event, init_func);
    gw_event_set_exec_task(&pnode->event, exec_task);
    gw_event_set_exec_args_task(&pnode->event, exec_args_task);
    gw_event_set_timer(&pnode->event, &global_timer);
    gw_event_set_timestamp(&pnode->event, global_timer.timestamp);
    gw_msg_init(&pnode->event.msg,task_id);

    gw_list_insert_node_first(ptask_list, pnode);
}

/**
 * @brief Creates a callback function without parameters for the task
 * @param name      - pointer to ths string of task name
 * @param taskid    - task id
 * @param type      - task type, see gw_type.h
 * @param status    - task status, see gw_type.h
 * @param poll_time - time circle to schedule task
 * @param priority  - priority of task, low value is high priority
 * @param init_func - callback when initialization
 * @param p_exec_func - callback without args
 */
void gw_task_void_create(const char *name, DATA_TYPE taskid,uint8_t type,
                    uint8_t status, uint32_t poll_time,
                    p_init_func init_func,
                    p_exec_func exec_func){
    gw_task_create(name,taskid,type,status,poll_time,0,
                   init_func,exec_func, NULL);
}

/**
 * @brief Creates a callback function with parameters for the task
 * @param name      - pointer to ths string of task name
 * @param taskid    - task id
 * @param type      - task type, see gw_type.h
 * @param status    - task status, see gw_type.h
 * @param poll_time - time circle to schedule task
 * @param priority  - priority of task, low value is high priority
 * @param init_func - callback when initialization
 * @param p_exec_args_func - callback with args
 */
void gw_task_args_create(const char *name, DATA_TYPE taskid,uint8_t type,
                    uint8_t status, uint32_t poll_time,
                    p_init_func init_func,
                    p_exec_args_func exec_args_func){

    gw_task_create(name,taskid,type,
                   status,poll_time,0,
                   init_func, NULL, exec_args_func);
}
					
/**
 * @brief   The initialization task will be executed
 */
void gw_task_init_process(void){
    gw_list* plist_tmp = NULL;
    struct gw_event *p_event = NULL;
    if(ptask_list->data < 1){
        return;
    }
    //GW_ENTER_CRITICAL_AREA;
    plist_tmp = ptask_list->next;
    while(plist_tmp != NULL){
        p_event = &plist_tmp->event;
        if(p_event->init_task != NULL){
            p_event->init_task();
        }
        plist_tmp = plist_tmp->next;
    //GW_EXIT_CRITICAL_AREA;
    }
}
/**
 * @brief   Task execution function
 *          The ready task will be executed
 */
void gw_task_process(void){
    gw_list* plist_tmp = NULL;
    struct gw_event *p_event = NULL;
    if(ptask_list->data < 1){
        return;
    }
    //GW_ENTER_CRITICAL_AREA;
    plist_tmp = ptask_list->next;
    while(plist_tmp != NULL){
        p_event = &plist_tmp->event;
        if(p_event->status == GW_ENABLE ){
            if( p_event->exec_task != NULL){
                p_event->exec_task();
            }
            if(p_event->exec_args_task != NULL){
                p_event->exec_args_task((struct gw_event *)p_event);
            }
            p_event->status = GW_DISABLE;
        }
        plist_tmp = plist_tmp->next;
    //GW_EXIT_CRITICAL_AREA;
    }
}

/**
 * @brief   Task scheduling function
 *          if task is ready, status will be set GW_ENABLE
 */
void gw_task_schedule(void){
    gw_list* plist_tmp = NULL;
    struct gw_event *p_event = NULL;
    if(ptask_list->data < 1){
        return;
    }
    GW_ENTER_CRITICAL_AREA;
    plist_tmp = ptask_list->next;
    while(plist_tmp != NULL){
        p_event = &plist_tmp->event;
        if(p_event->poll_time + p_event->timestamp < p_event->g_timer->timestamp ){
            p_event->timestamp = p_event->g_timer->timestamp;
            p_event->status = GW_ENABLE;
        }
        plist_tmp = plist_tmp->next;
    }
    GW_EXIT_CRITICAL_AREA;
}

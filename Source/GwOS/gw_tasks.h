/**
************************************************************************************************
 * @file    : gw_tasks.h
 * @brief   : none
 * @details : none
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
 *      This dll is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program.If not, see < https://www.gnu.org/licenses/>.
*************************************************************************************************
*/

#ifndef GW_TASKS_H
#define GW_TASKS_H
#include "gw_event.h"

/**
 * @brief Initialize the task list
 */
void gw_task_list_init(void);

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
void gw_task_create(const char *name, DATA_TYPE taskid,uint8_t type,
                    uint8_t status, uint32_t poll_time,uint32_t priority,
                    p_init_func init_func,
                    p_exec_func exec_task,
                    p_exec_args_func exec_args_task);

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
void gw_task_void_create(const char *name, DATA_TYPE taskid,uint8_t type, uint8_t status, uint32_t poll_time,
                    p_init_func init_func,
                    p_exec_func exec_func);

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
void gw_task_args_create(const char *name, DATA_TYPE taskid,uint8_t type, uint8_t status, uint32_t poll_time,
                         p_init_func init_func,
                         p_exec_args_func exec_func);

/**
 * @brief   Task scheduling function
 *          if task is ready, status will be set GW_ENABLE
 */
void gw_task_schedule(void);

/**
 * @brief   Task execution function
 *          The ready task will be executed
 */
void gw_task_process(void);

/**
 * @brief   The initialization task will be executed
 */
void gw_task_init_process(void);

#endif


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
#include "gw_msgs.h"

gw_msg_fifo_t msg_fifo;

/**
 * @brief Initialize the ring fifo queue
 */
void gw_msg_fifo_init(gw_msg_fifo_t * const pfifo){
	pfifo->size = MSGS_FIFO_SIZE;
	pfifo->front = pfifo->rear = 0;
    for(int i = 0; i<MSGS_FIFO_SIZE; i++){
        pfifo->msgs[i].pstr = NULL;
    }
}


/**
 * @brief Gets a pointer to the global message queue
 */
gw_msg_fifo_t* gw_msg_fifo_get_pointer(void){
	return &msg_fifo;
}


/**
 * @brief Empty the ring fifo queue
 */
void gw_msg_fifo_clear(gw_msg_fifo_t * const pfifo){
	pfifo->front = pfifo->rear = 0;
    for(int i = 0; i<MSGS_FIFO_SIZE; i++){
        pfifo->msgs[i].pstr = NULL;
    }
}

/**
 * @brief   Initialize a single message
 * @param   pmsg    - pointer to the msg
 * @param   id      - the id of task
 */
void gw_msg_init(gw_msg_t* const pmsg,DATA_TYPE id){
    pmsg->pstr = NULL;
    pmsg->task_id = id;
}

/**
 * @brief   Check if the queue is full
 * @param   pfifo   - pointer to the fifo
 * @return  GW_RESULT
 */
GW_RESULT gw_msg_fifo_is_full(gw_msg_fifo_t* const pfifo){

    if( (pfifo->rear + 1) % pfifo->size ==  pfifo->front){
		return GW_TRUE;
	}
	return GW_FALSE;
}

/**
 * @brief   Check if the queue is empty
 * @param   pfifo   - pointer to the fifo
 * @return  GW_RESULT
 */
GW_RESULT gw_msg_fifo_is_empty(gw_msg_fifo_t* const pfifo){
	if(pfifo->front == pfifo->rear){
		return GW_TRUE;
	}
	return GW_FALSE;
}

/**
 * @brief   Enqueue up for a message
 * @param   pfifo   - pointer to the fifo
 * @param   pmsg    - the pointer to the intput message
 * @return  GW_RESULT
 */
GW_RESULT gw_msg_fifo_enqueue(gw_msg_fifo_t* const pfifo,gw_msg_t* const pmsg){
	//判断队列非满可入队
	if(gw_msg_fifo_is_full(pfifo) == GW_TRUE){
		return GW_ERROR;
	}
    pfifo->msgs[pfifo->rear++%pfifo->size] = *pmsg;
	return GW_SUCCESS;
}

/**
 * @brief   Queue up for a message
 * @param   pfifo   - pointer to the fifo
 * @param   pmsg    - the pointer to the output message
 * @return  GW_RESULT
 */
GW_RESULT gw_msg_fifo_dequeue(gw_msg_fifo_t* const pfifo,gw_msg_t* const pmsg){

	//判断队列非空可出队
	if(gw_msg_fifo_is_empty(pfifo) == GW_TRUE){
		return GW_ERROR;
	}
	*pmsg = pfifo->msgs[pfifo->front++ % pfifo->size];
	return GW_SUCCESS;
}

/**
 * @brief   Sends a message to the task with the specified ID
 * @param   taskid  - the specified ID of the target task
 * @param   pfifo   - pointer to the fifo
 * @param   pmsg    - the pointer to the message will be sent
 * @return  GW_RESULT
 */
GW_RESULT gw_msg_send_msg(uint32_t taskid, gw_msg_fifo_t* const pfifo,
                      gw_msg_t* const pmsg){

    if(gw_msg_fifo_is_full(pfifo) == GW_TRUE){
        return GW_ERROR;
    }
    pmsg->task_id = taskid;
    gw_msg_fifo_enqueue(pfifo, pmsg);

    return GW_SUCCESS;
}

/**
 * @brief   Sends a string to the task with the specified ID
 * @param   taskid  - the specified ID of the target task
 * @param   pfifo   - pointer to the fifo
 * @param   pmsg    - the pointer to the string will be sent
 * @return  GW_RESULT
 */
GW_RESULT gw_msg_send_str(uint32_t taskid, gw_msg_fifo_t* const pfifo,
                      const char *pstr){

    if(gw_msg_fifo_is_full(pfifo) == GW_TRUE){
        return GW_ERROR;
    }
    gw_msg_t msg;
    msg.pstr = (char *)pstr;
    msg.task_id = taskid;
    gw_msg_fifo_enqueue(pfifo, &msg);

    return GW_SUCCESS;
}

/**
 * @brief   Receives a message with the specified task ID from the message queue
 * @param   taskid  - the specified ID of the target task
 * @param   pfifo   - pointer to the fifo
 * @param   pmsg    - pmsg    - the pointer to the output message
 * @return  GW_RESULT
 */
GW_RESULT gw_msg_receive(uint32_t taskid, gw_msg_fifo_t* const pfifo,
                         gw_msg_t* const pmsg){

    gw_msg_t* pmsg_tmp = NULL;
    uint8_t front = 0;
    if(gw_msg_fifo_is_empty(pfifo) == GW_TRUE){
        pmsg->pstr = NULL;
        pmsg->task_id = 0;
        return GW_ERROR;
    }
    front = pfifo->front%pfifo->size;

    pmsg_tmp = &(pfifo->msgs[front]);
    if(pmsg_tmp->task_id == taskid){
        //*pmsg = pfifo->msgs[front];
        gw_msg_fifo_dequeue(pfifo, pmsg);
        return GW_SUCCESS;
    }else{
        pmsg->pstr = NULL;
        return GW_ERROR;
    }
}


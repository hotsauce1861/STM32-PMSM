#include "gw_msgs.h"

void gw_msg_fifo_init(gw_msg_fifo_mod_typedef * const pfifo){
	pfifo->size = MSGS_FIFO_SIZE;
	pfifo->front = pfifo->rear = 0;
}

void gw_msg_fifo_clear(gw_msg_fifo_mod_typedef * const pfifo){
	pfifo->front = pfifo->rear = 0;
}

GW_RESULT gw_msg_fifo_is_full(gw_msg_fifo_mod_typedef* const pfifo){

	if( (pfifo->rear + 1) %pfifo->size ==  pfifo->front){
		return GW_TRUE;
	}
	return GW_FALSE;
}

GW_RESULT gw_msg_fifo_is_empty(gw_msg_fifo_mod_typedef* const pfifo){
	if(pfifo->front == pfifo->rear){
		return GW_TRUE;
	}
	return GW_FALSE;
}

GW_RESULT gw_msg_fifo_enqueue(gw_msg_fifo_mod_typedef* const pfifo, 
								gw_msg_mod_typedef* const pmsg){
	//判断队列非满可入队
	if(gw_msg_fifo_is_full(pfifo) == GW_TRUE){
		return GW_ERROR;
	}
	pfifo->msgs[++pfifo->rear%pfifo->size] = *pmsg;
	return GW_SUCCESS;
}

GW_RESULT gw_msg_fifo_dequeue(gw_msg_fifo_mod_typedef* const pfifo,
								gw_msg_mod_typedef* const pmsg){

	//判断队列非空可出队
	if(gw_msg_fifo_is_empty(pfifo) == GW_TRUE){
		return GW_ERROR;
	}
	*pmsg = pfifo->msgs[pfifo->front++ % pfifo->size];
	return GW_SUCCESS;
}




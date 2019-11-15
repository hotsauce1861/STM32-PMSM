#include "gw_list.h"
#include "gw_type.h"

gw_list* gw_list_init(void){
	gw_list* phead = (gw_list*)GW_MALLOC(sizeof(gw_list_node));
	phead->next = NULL;
	phead->data = 0;
	return phead;
}

gw_list_node* gw_list_node_init(DATA_TYPE data){
    gw_list_node* pnode = (gw_list*)GW_MALLOC(sizeof(gw_list_node));
    pnode->next = NULL;
    pnode->data = data;
    return pnode;
}

/**
 *@brief insert a node behind head node and before second node
 */
GW_RESULT gw_list_insert_node_last(gw_list* plist, gw_list_node* const pnode){
	if(plist == NULL){
		return GW_ERROR;
	}
	plist->data++;
    while(plist->next != NULL){
		plist = plist->next;		
	}
    plist->next = pnode;
	return GW_SUCCESS;
}
/**
 *@brief insert a node behind head node and before second node
 */
GW_RESULT gw_list_insert_node_first(gw_list* plist, gw_list_node* const pnode){
	if(plist == NULL){
		return GW_ERROR;
	}
	plist->data++;
	pnode->next = plist->next;
	plist->next = pnode;
	return GW_SUCCESS;
}

/**
 * @brief insert a node behind head node and before second node
 * @arg index 0-max_list_num
 */
GW_RESULT gw_list_insert_node_by_index(gw_list* plist, gw_list_node* const pnode, uint8_t index){

	uint8_t i = 0;
	if(plist == NULL){
		return GW_ERROR;
	}
	if(plist->data < index){
		return GW_ERROR;
	}
	plist->data++;
	while(i++<index){
		plist = plist->next;		
	}
	pnode->next = plist->next;
    plist->next = pnode;
	return GW_SUCCESS;
}

/**
 *@brief remove last node in list
 */
GW_RESULT gw_list_remove_node_last(gw_list* plist){

	gw_list_node* pnode_tmp = NULL;
	if(plist == NULL){
		return GW_ERROR;
	}
	if(plist->data < 1){
		return GW_ERROR;
	}
	plist->data--;
	while(plist->next != NULL){
		pnode_tmp = plist;
		plist = plist->next;
	}
	pnode_tmp->next = NULL;	
    gw_list_node_free(plist);
    return GW_SUCCESS;
}

/**
 * @brief remove a node behind head node and before second node
 */
GW_RESULT gw_list_remove_node_first(gw_list* const plist){
	gw_list_node* pnode_tmp = NULL;
	if(plist == NULL){
		return GW_ERROR;
	}
	if(plist->data < 1){
		return GW_ERROR;
	}
	plist->data--;
	
	pnode_tmp = plist->next;
	plist->next = pnode_tmp->next;
    gw_list_node_free(pnode_tmp);
    return GW_SUCCESS;
}
/**
 * @brief remove a node by index
 * @arg index 0 - max_index_num-1
 */
GW_RESULT gw_list_remove_node_by_index(gw_list* plist, uint8_t index){
	uint16_t i = 0;
	gw_list_node* pnode_tmp = NULL;
	if(plist == NULL){
		return GW_ERROR;
	}
	if(plist->data < index){
		return GW_ERROR;
	}
	plist->data--;
	while(i++<index){
		plist = plist->next;
	}
	pnode_tmp = plist->next;
	plist->next = pnode_tmp->next;
	gw_list_node_free(pnode_tmp);
    return GW_SUCCESS;
}
/**
 * @brief free memory of one node
 */
GW_RESULT gw_list_node_free(gw_list_node* const pnode){

	if(pnode == NULL){
		return GW_ERROR;
	}
	GW_FREE(pnode);
	return GW_SUCCESS;
}

/**
 * @brief free memory of list
 */
GW_RESULT gw_list_free(gw_list* plist){

	gw_list_node* pnode_tmp = NULL;

	if(plist == NULL){
		return GW_ERROR;
	}	
	while(plist->next != NULL){
		pnode_tmp = plist;
		plist =	plist->next;
		gw_list_node_free(pnode_tmp);
	}
	return GW_SUCCESS;
}
/**
 * @brief get list node length
 */
DATA_TYPE gw_list_length(gw_list* const plist){
	return plist->data;
}





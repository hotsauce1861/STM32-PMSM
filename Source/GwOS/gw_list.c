/**
************************************************************************************************
 * @file    : gw_list.c
 * @brief   :
 * @details :
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

#include "gw_list.h"
#include "gw_type.h"

/**
 * @brief   Initialization list
 * @return  gw_list     - The pointer to the list
 */
gw_list* gw_list_init(void){
	//gw_list* phead = (gw_list*)GW_MALLOC(sizeof(gw_list_node));
	gw_list* phead = (gw_list*)malloc(sizeof(gw_list_node));
	if(phead == NULL){
		return NULL;
	}
	phead->next = NULL;
	phead->data = 0;
	return phead;
}

/**
 * @brief   Initialization list node
 * @return  gw_list     - The pointer to the list node
 */
gw_list_node* gw_list_node_init(DATA_TYPE data){
    gw_list_node* pnode = (gw_list*)GW_MALLOC(sizeof(gw_list_node));
    pnode->next = NULL;
    pnode->data = data;
    return pnode;
}

/**
 * @brief insert a node behind head node and before second node
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
 * @brief insert a node behind head node and before second node
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
 * @param plist
 * @param gw_list_node
 * @param index
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
 * @brief remove last node in list
 * @param plist
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
 * @param plist
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
 * @param plist
 * @param index
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
 * @brief   free memory of one node
 * @param   pnode
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




/**
************************************************************************************************
 * @file    : gw_list.h
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
 *      This dll is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program.If not, see < https://www.gnu.org/licenses/>.
*************************************************************************************************
*/

#ifndef GW_LIST_H
#define GW_LIST_H

#include "gw_event.h"
#include "gw_type.h"
struct gw_list_node{
    struct gw_list_node *next;
    gw_event_t event;
    DATA_TYPE data;
};

typedef struct gw_list_node gw_list_node;
typedef struct gw_list_node gw_list;

gw_list* gw_list_init(void);
gw_list_node* gw_list_node_init(DATA_TYPE data);
GW_RESULT gw_list_insert_node_last(gw_list* const plist, gw_list_node* const pnode);
GW_RESULT gw_list_insert_node_first(gw_list* const plist, gw_list_node* const pnode);
GW_RESULT gw_list_insert_node_by_index(gw_list* const plist, gw_list_node* const pnode, uint8_t index);
GW_RESULT gw_list_remove_node_last(gw_list* plist);
GW_RESULT gw_list_remove_node_first(gw_list* const plist);
GW_RESULT gw_list_remove_node_by_index(gw_list* plist, uint8_t index);
GW_RESULT gw_list_free(gw_list* plist);
GW_RESULT gw_list_node_free(gw_list_node* const pnode);
GW_RESULT gw_list_is_empty(gw_list* const plist);
DATA_TYPE gw_list_length(gw_list* const plist);

#endif

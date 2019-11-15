#ifndef GW_LIST_H
#define GW_LIST_H

#include "gw_event.h"

#define DATA_TYPE   uint32_t

struct gw_list_node{
    struct gw_list_node *next;
    gw_event_typedef event;
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


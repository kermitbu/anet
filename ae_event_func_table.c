#include "ae_event_func_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

event_func_table_t* create_event_func_table(int barrel) {
	
    event_func_table_t* table = malloc(sizeof(event_func_table_t));

    table->barrel = barrel;
    int size = sizeof(event_func_node_t*) * barrel;
    table->event_func_node_list = malloc(size);
    memset(table->event_func_node_list, 0, size);

    return table;
}

void free_event_func_table(event_func_table_t** table) {

    for (int i = 0; i < (*table)->barrel; i++) {
        event_func_node_t* node = (*table)->event_func_node_list[i];
        event_func_node_t* p = NULL;
        while (node) {
            p = node->next;
            free(node);
            node = p;
        }
    }

    free(*table);
    *table = NULL;
}

event_func_node_t* create_event_func_node(int event, void* func, void* data, event_func_node_t* next) {
    event_func_node_t* node = malloc(sizeof(event_func_node_t));
    node->event_id = event;
    node->func = func;
    node->data = data;
    node->next = next;
    return node;
}

void insert_event_func(event_func_table_t* table, int event, void* func, void* data) {
    int idx = event % table->barrel;
    if (table->event_func_node_list[idx]) {
        table->event_func_node_list[idx] = create_event_func_node(event, func, data, table->event_func_node_list[idx]);
    } else {
        table->event_func_node_list[idx] = create_event_func_node(event, func, data, NULL);
    }
}

void* find_event_func(event_func_table_t* table, int event) {
    int idx = event % table->barrel;
    if (table->event_func_node_list[idx]) {
        event_func_node_t* node = table->event_func_node_list[idx];
        do {
            if (node->event_id == event) {
                return node->func;
            }
        } while (node = node->next);
    }

    return NULL;
}


event_func_node_t* find_event_func_node(event_func_table_t* table, int event) {
    int idx = event % table->barrel;
    if (table->event_func_node_list[idx]) {
        event_func_node_t* node = table->event_func_node_list[idx];
        do {
            if (node->event_id == event) {
                return node;
            }
        } while (node = node->next);
    }

    return NULL;
}


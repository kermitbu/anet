#ifndef AE_EVENT_FUNC_TABLE_H
#define AE_EVENT_FUNC_TABLE_H


#ifdef __cplusplus
extern "C" {
#endif

typedef struct event_func_node_t event_func_node_t;
typedef struct event_func_node_t {
	int event_id;
	void* func;
	void* data;
	event_func_node_t* next;
}event_func_node_t;


typedef struct event_func_table_t {
	int barrel;
	event_func_node_t** event_func_node_list;
}event_func_table_t;


event_func_table_t* create_event_func_table(int barrel);
void free_event_func_table(event_func_table_t** table);
event_func_node_t* create_event_func_node(int event, void* func, void* data, event_func_node_t* next);
void insert_event_func(event_func_table_t* table, int event, void* func, void* data);
void* find_event_func(event_func_table_t* table, int event);
event_func_node_t* find_event_func_node(event_func_table_t* table, int event);
#ifdef __cplusplus
}
#endif

#endif //AE_EVENT_FUNC_TABLE_H
#ifndef ANET_SERVER_H
#define ANET_SERVER_H

#include "define.h"
#include "anet.h"
#include "ae.h"
#include "buffer.h"
#include "protocol.h"
#include "ae_event_func_table.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    aeEventLoop* loop; //最核心的事件循环
    int listen_fd; //监听fd，socket函数返回
    char host[64];
    int port; //默认的监听端口
    int backlog; //listen函数第二个参数backlog的大小
    int max_client_count; //最大的客户端连接数
    char err_info[ANET_ERR_LEN]; //err信息
    event_func_table_t *handler_table;
} server_t;

typedef struct {
    aeEventLoop *loop;
    event_func_table_t *handler_table;
    int fd;
    buffer_t *read_buffer;
    buffer_t *write_buffer;
} client_t;

typedef int(*handle_ptr_t)(void*, client_t*, package_t*);

#define ea_register_event_handler(map, id, func, data)  do{insert_event_func(map, id, (void*)(func), data);}while(0)

void init_server(server_t *server);
void wait_server(server_t *server);
int ae_server_run(event_func_table_t *handler_table);
int send_package(client_t* client, package_t* package, int trusted);

#ifdef __cplusplus
}
#endif

#endif //ANET_SERVER_H

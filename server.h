#ifndef ANET_SERVER_H
#define ANET_SERVER_H

#include "define.h"
#include "anet.h"
#include "ae.h"
#include "buffer.h"

typedef struct {
    aeEventLoop* loop; //最核心的事件循环
    int listen_fd; //监听fd，socket函数返回
    int port; //默认的监听端口
    int backlog; //listen函数第二个参数backlog的大小
    int max_client_count; //最大的客户端连接数
    char err_info[ANET_ERR_LEN]; //err信息
} server_t;

typedef struct {
    aeEventLoop *loop;
    int fd;
    buffer_t *read_buffer;
    buffer_t *write_buffer;
} client_t;

void init_server(server_t *server);
void wait_server(server_t *server);

#endif //ANET_SERVER_H

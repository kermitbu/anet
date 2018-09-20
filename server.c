#include "server.h"
#include "buffer.h"
#include "protocol.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

static client_t *alloc_client()
{
    client_t * client = zmalloc(sizeof(client_t));
    if (client == NULL) {
        goto err;
    }
    client->loop = NULL;
    client->fd = -1;
    client->read_buffer = alloc_buffer();
    client->write_buffer = alloc_buffer();
    if (client->read_buffer == NULL || client->write_buffer == NULL) {
        goto err;
    }

    return client;

err:
    if (client) {
        zfree(client->read_buffer);
        zfree(client->write_buffer);
        zfree(client);
    }

    return NULL;
}

static void free_client(client_t *client)
{
    if (client) {
        if (client->fd > 0) {
            aeDeleteFileEvent(client->loop, client->fd, AE_READABLE);
            aeDeleteFileEvent(client->loop, client->fd, AE_WRITABLE);
            close(client->fd);
        }
        zfree(client->read_buffer);
        zfree(client->write_buffer);
        zfree(client);
    }
}

static void writeEventHandler(aeEventLoop *loop, int fd, void *data, int mask)
{
    client_t *client = (client_t *)data;
    buffer_t *wbuffer = client->write_buffer;

    int data_size = (int)get_readable_size(wbuffer);
    if (data_size == 0) {
        aeDeleteFileEvent(client->loop, client->fd, AE_WRITABLE);
        return;
    }

    int writen = anetWrite(client->fd, (char *)wbuffer->buff + wbuffer->read_idx, data_size);
    if (writen > 0) {
        wbuffer->read_idx += writen;
    }
    if (get_readable_size(wbuffer) == 0) {
        aeDeleteFileEvent(client->loop, client->fd, AE_WRITABLE);
    }
}

static void readEventHandler(aeEventLoop *loop, int fd, void *data, int mask)
{
    client_t *client = (client_t *)data;
    buffer_t *rbuffer = client->read_buffer;

    check_buffer_size(rbuffer, DEFAULT_BUFF_SIZE / 2);

    size_t avlid_size = rbuffer->size - rbuffer->write_idx;
    ssize_t readn = read(fd, rbuffer->buff + rbuffer->write_idx, avlid_size);

    if (readn > 0) {
        rbuffer->write_idx += readn;
        package_t *req_package = NULL;
        while (1) {
            int decode_ret = packet_decode(rbuffer, &req_package);
            if (decode_ret == 0) {
                package_t *resp_package = NULL;
                do_package(req_package, &resp_package);
                if (resp_package) {
                    buffer_t *wbuffer = client->write_buffer;
                    check_buffer_size(wbuffer, sizeof(package_head_t) + resp_package->head.length);
                    package_encode(wbuffer, resp_package);
                    int writen = anetWrite(client->fd, (char *)wbuffer->buff + wbuffer->read_idx,
                                           (int)get_readable_size(wbuffer));
                    if (writen > 0) {
                        wbuffer->read_idx += writen;
                    }
                    if (get_readable_size(wbuffer) != 0) {
                        if (aeCreateFileEvent(client->loop, client->fd, AE_WRITABLE, writeEventHandler, client) == AE_ERR) {
                            printf("create socket writeable event error, close it.");
                            free_client(client);
                        }
                    }
                }
                zfree(req_package);
                zfree(resp_package);
            } else if (decode_ret == -1) {
                // not enough data
                break;
            } else if (decode_ret == -2) {
                char ip_info[64];
                anetFormatPeer(client->fd, ip_info, sizeof(ip_info));
                printf("decode error, %s disconnect.\n", ip_info);
                // recv wrong data, disconnect
                free_client(client);
            }
        }
    } else if (readn == 0) {
        printf("client disconnect, close it.\n");
        free_client(client);
    }
}

static void acceptTcpHandler(aeEventLoop *loop, int fd, void *data, int mask)
{
    char cip[64];
    int cport;

    server_t *server = (server_t *)data;
    // 调用accept接受连接
    int cfd = anetTcpAccept(NULL, fd, cip, sizeof(cip), &cport);
    if (cfd != -1) {
        printf("accepted ip %s:%d\n", cip, cport);
        anetNonBlock(NULL, cfd);                // 设置socket非阻塞
        anetEnableTcpNoDelay(NULL, cfd);        // 开启TcpNoDelay选项
        client_t *client = alloc_client();      // 申请一个新的客户端
        if (!client) {
            printf("alloc client error...close socket\n");
            close(fd);
            return;
        }

        client->loop = loop;
        client->fd = cfd;

        if (aeCreateFileEvent(loop, cfd, AE_READABLE, readEventHandler, client) == AE_ERR) {
            // //继续调用aeCreateFileEvent给新连接的fd注册可读事件，并且注册读函数readEventHandler
            if (errno == ERANGE) {
                // or use aeResizeSetSize(server->loop, cfd) modify this limit
                printf("so many client, close new.");
            } else {
                printf("create socket readable event error, close it.");
            }
            free_client(client);
        }
    }
}

void init_server(server_t *server) {

    // 为loop中各类数据结构申请空间
    server->loop = aeCreateEventLoop(server->max_client_count);

    // 创建listen_fd 实际上调用socket函数 
    server->listen_fd = anetTcpServer(server->err_info, server->port, NULL, server->backlog);
    if (server->listen_fd != ANET_ERR) {
        // 设置非阻塞
        anetNonBlock(server->err_info, server->listen_fd);
    }

    // 将 listen_fd 注册到epoll的实例上，事件处理函数为acceptTcpHandler
    if (aeCreateFileEvent(server->loop, server->listen_fd, AE_READABLE, acceptTcpHandler, server) != AE_ERR) {
        char conn_info[64];
        anetFormatSock(server->listen_fd, conn_info, sizeof(conn_info));
        printf("listen on: %s\n", conn_info);
    }
}

void wait_server(server_t *server)
{
    // 循环处理
    aeMain(server->loop);

    // 退出循环后，删除loop
    aeDeleteEventLoop(server->loop);
}

int main()
{
    // 忽略SIGPIPE信号，防止给一个已经关闭socket的客户端连续两次发送数据导致SIGPIPE信号产生
    signal(SIGPIPE, SIG_IGN);

    //创建一个server
    server_t server;
    memset(&server, 0x00, sizeof(server));

    //设置backlog的大小
    server.backlog = DEFAULT_LISTEN_BACKLOG;

    //设置最大的客户端连接数
    server.max_client_count = DEFAULT_MAX_CLIENT_COUNT;

    //设置默认的监听端口
    server.port = DEFAULT_LISTEN_PORT;

    //初始化
    init_server(&server);

    //实际上进入了loop循环
    wait_server(&server);

    return 0;
}

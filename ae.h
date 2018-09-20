#ifndef ANET_AE_H
#define ANET_AE_H

#include <time.h>

#define AE_OK        0
#define AE_ERR      -1

#define AE_NONE      0
#define AE_READABLE  1
#define AE_WRITABLE  2

#define AE_FILE_EVENTS  1
#define AE_TIME_EVENTS  2
#define AE_ALL_EVENTS    (AE_FILE_EVENTS|AE_TIME_EVENTS)
#define AE_DONT_WAIT    4

#define AE_NOMORE            -1
#define AE_DELETED_EVENT_ID  -1

/* Macros */
#define AE_NOTUSED(V) ((void) V)

struct aeEventLoop;

/* Types and data structures */
typedef void aeFileProc(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask);

typedef int aeTimeProc(struct aeEventLoop *eventLoop, long long id, void *clientData);

typedef void aeEventFinalizerProc(struct aeEventLoop *eventLoop, void *clientData);

typedef void aeBeforeSleepProc(struct aeEventLoop *eventLoop);

// 文件事件结构体
typedef struct aeFileEvent {
    int mask;       /* one of AE_(READABLE|WRITABLE) */
    aeFileProc *rfileProc;
    aeFileProc *wfileProc;
    void *clientData;
} aeFileEvent;

// 定时事件结构体
typedef struct aeTimeEvent {
    long long id;       // 定时事件ID
    long when_sec;      // 秒
    long when_ms;       // 毫秒
    aeTimeProc *timeProc;   // 定时事件处理程序
    aeEventFinalizerProc *finalizerProc;    // 事件处理函数
    void *clientData;           // 事件处理数据
    struct aeTimeEvent *next;   // 下一个节点
} aeTimeEvent;

// 触发事件结构体
typedef struct aeFiredEvent {
    int fd;         // 描述符
    int mask;       // 发生事件的掩码（读/写）
} aeFiredEvent;

/* State of an event based program */
typedef struct aeEventLoop {
    int maxfd;                  // 当前注册的最大文件描述符
    int setsize;                // 监控的最大文件描述符数
    long long timeEventNextId;  // 定时事件ID 
    time_t lastTime;            // 最近一次处理定时事件的时间
    aeFileEvent *events;        // 注册事件链表
    aeFiredEvent *fired;        // 触发事件链表
    aeTimeEvent *timeEventHead; // 定时事件链表
    int stop;                   // 是否停止循环
    void *apidata;              // 特定接口的特定数据
    aeBeforeSleepProc *beforesleep;     // 在sleep之前执行的程序
} aeEventLoop;

aeEventLoop *aeCreateEventLoop(int setsize);

void aeDeleteEventLoop(aeEventLoop *eventLoop);

void aeStop(aeEventLoop *eventLoop);

int aeCreateFileEvent(aeEventLoop *eventLoop, int fd, int mask, aeFileProc *proc, void *clientData);

void aeDeleteFileEvent(aeEventLoop *eventLoop, int fd, int mask);

int aeGetFileEvents(aeEventLoop *eventLoop, int fd);

long long aeCreateTimeEvent(aeEventLoop *eventLoop, long long milliseconds,
                            aeTimeProc *proc, void *clientData, aeEventFinalizerProc *finalizerProc);

int aeDeleteTimeEvent(aeEventLoop *eventLoop, long long id);

int aeProcessEvents(aeEventLoop *eventLoop, int flags);

void aeMain(aeEventLoop *eventLoop);

int aeWait(int fd, int mask, long long milliseconds);

char *aeGetApiName(void);

void aeSetBeforeSleepProc(aeEventLoop *eventLoop, aeBeforeSleepProc *beforesleep);

int aeGetSetSize(aeEventLoop *eventLoop);

int aeResizeSetSize(aeEventLoop *eventLoop, int setsize);

#endif //ANET_AE_H

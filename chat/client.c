
#include "anet.h"
#include "ae.h"

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>

#define CHAT_CONNECTED 0x1
#define CHAT_DISCONNECTING 0x2

typedef struct chatAeContext {
    int flags;
} chatAeContext;

typedef struct chatAeEvents {
    chatAeContext *context;
    aeEventLoop *loop;
    int fd;
    int reading, writing;
} chatAeEvents;


int chatCheckConnectDone(chatAeContext *c, int *completed) {
    int rc = connect(c->fd, )
}

static void chatAeReReadEvent(aeEventLoop *el, int fd, void *privdata, int mask) {
    char buf[1024*16];
    int nread;

    chatAeEvents *e = (chatAeEvents*)privdata;
    nread = read(fd, buf, sizeof(buf));
    if(nread < 0) {
        if(errno == EAGAIN) {
            return;
        }
    }
    else if(nread == 0) {
        if(fd != STDIN_FILENO)
            e->context->flags |= CHAT_DISCONNECTING;
    }
    else {
        if(fd != STDIN_FILENO)
            write(STDOUT_FILENO, buf, sizeof(buf));
    }
}

static void chatAeAddRead(void *privData) {
    chatAeEvents *e = (chatAeEvents*)privData;
    aeEventLoop *loop = e->loop;
    if (!e->reading) {
        e->reading = 1;
        aeCreateFileEvent(loop, e->fd, AE_READABLE, chatAeReReadEvent, e);
    }
}

static void chatAeWriteEvent(aeEventLoop *el, int fd, void *privdata, int mask) {
    char buf[1024*16];
    int nread;

    chatAeEvents *e = (chatAeEvents*)privdata;
    if(fd == STDOUT_FILENO) {
        nread = read(fd, buf, sizeof(buf));
        if(nread < 0) {
            if(errno == EAGAIN) {
                return;
            }
        }
        else if(nread == 0) {
            fprintf(stderr, "EOF on stdin\n");
        }
        else {
            write(e->fd, buf ,sizeof(buf));
        }
    }
    e->context->flags |= CHAT_CONNECTED;

    chatAeAddRead(e);
}

static void chatAeAddWrite(void *privdata) {
    chatAeEvents *e = (chatAeEvents*)privdata;
    aeEventLoop *loop = e->loop;
    if(!e->writing) {
        e->writing = 1;
        aeCreateFileEvent(loop, e->fd, AE_WRITABLE, chatAeWriteEvent, e);
    }
}

static void asyncConnect(chatAeEvents *events) {
    int fd;
    char *err;

    if((fd = anetTcpNonBlockConnect(err, "127.0.0.1", 6379)) == ANET_ERR){
        printf("connect server error.%s", err);
    }

    events->fd = fd;
    chatAeAddWrite((void *)events);
    aeCreateFileEvent(events->loop, STDIN_FILENO, AE_WRITABLE, chatAeReReadEvent, events);
}


int main(int argc, const char *argv[])
{

    chatAeEvents *events;

    events = (chatAeEvents*)malloc(sizeof(events));
    events->loop = aeCreateEventLoop(50);
    asyncConnect(events);

    aeMain(events->loop);
    return 0;
}


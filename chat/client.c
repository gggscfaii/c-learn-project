
#include "anet.h"
#include "ae.h"

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>

#define CHAT_CONNECTED 0x1
#define CHAT_DISCONNECTING 0x2
#define CHAT_OK 1
#define CHAT_ERR 0

typedef struct chatAeContext {
    int flags;
    int fd;
    struct sockaddr *saddr;
    size_t *saddrlen;
} chatAeContext;

typedef struct chatAeEvents {
    chatAeContext *context;
    aeEventLoop *loop;
    int fd;
    int reading, writing;
} chatAeEvents;

int chatCheckConnectDone(chatAeContext *c, int *completed) {
    int rc = connect(c->fd, (struct sockaddr *)c->saddr, *c->saddrlen);
    if (rc == 0) {
       *completed = 1;
    }

    switch (errno) {
        case EISCONN:
            *completed = 1; 
            return CHAT_OK;
        case EALREADY:
        case EINPROGRESS:
        case EWOULDBLOCK:
            *completed = 0;
            return CHAT_OK;
        default:
            return CHAT_ERR;
            
    }
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
        if(fd != STDOUT_FILENO)
            e->context->flags |= CHAT_DISCONNECTING;
    }
    else {
        if(fd != STDOUT_FILENO)
            write(STDOUT_FILENO, buf, nread);
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
    int completed = 0;

    chatAeEvents *e = (chatAeEvents*)privdata;
    if(chatCheckConnectDone(e->context, &completed) == CHAT_ERR) {
        return;
    }

    if(completed == 0) {
        return;
    }

    if(fd == STDIN_FILENO) {
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
            write(e->fd,buf,nread);
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
    chatAeContext *context = events->context;

    events->context->saddr = malloc(16);
    events->context->saddrlen = (size_t*)malloc(sizeof(size_t));
    if((fd = anetTcpNonBlockConnect(err, "127.0.0.1", 6379, context->saddr, context->saddrlen)) == ANET_ERR){
        printf("connect server error.%s", err);
    }

    events->fd = fd;
    events->context->fd = fd;
    chatAeAddWrite((void *)events);
    aeCreateFileEvent(events->loop, STDIN_FILENO, AE_WRITABLE, chatAeWriteEvent, events);
}


int main(int argc, const char *argv[])
{

    chatAeEvents *events;

    events = (chatAeEvents*)malloc(sizeof(events));
    events->loop = aeCreateEventLoop(50);
    events->context = (chatAeContext*)malloc(sizeof(chatAeContext));
    asyncConnect(events);

    aeMain(events->loop);
    return 0;
}


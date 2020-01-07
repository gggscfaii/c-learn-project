
#include "anet.h"
#include "ae.h"

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

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


static void chatAeReReadEvent(aeEventLoop *el, int fd, void *privdata, int mask) {
    char buf[1024*16];
    int nread;

    chatAeEvents *e = (chatAeEvents*)privdata;
    nread = read(fd, buf, sizeof(buf));
    if(nread <= 0) {
        if(errno == EAGAIN) {
            return;
        }
        e->context->flags |=  
    }
}

static void chatAeWriteEvent(aeeventloop *el, int fd, void *privdata, int mask) {
    char buf[1024*16];
    int nread;

    chataeevents *e = (chataeevents*)privdata;
    nread = read(fd, buf, sizeof(buf));

}

static void chatAeAddRead(void *privData) {
   chatAeEvents *e = (chatAeEvents*)privData;
   aeEventLoop *loop = e->loop;
   if (!e->reading) {
       e->reading = 1;
       aeCreateFileEvent(loop, e->fd, AE_READABLE, chatAeWriteEvent, e);
   }
}

static void chatAeAddWrite(void *privdata) {
    chatAeEvents *e = (chatAeEvents*)privdata;
    aeEventLoop *loop = e->loop;
    if(!e->writing) {
        e->writing = 1;
        aeCreateFileEvent(loop, e->fd, AE_WRITABLE, chatAeAddWrite, e);
    }
}

int fd;
static void asyncConnect() {
    char *err;
    if((fd = anetTcpConnect(err, "127.0.0.1", 6379)) == ANET_ERR){
        printf("connect server error.");
    }

    if(anetNonBlock(err, fd) == ANET_ERR) {
        printf("anet not block error");
    }
}

static void *readData(void *arg) {
    int n;
    char buf[1024];
    while(1) {
       if((n=read(fd, buf, 1024)) < 0) {
            continue;    
       }
       write(STDOUT_FILENO, buf, n);
   }
   return NULL; 
}

int main(int argc, const char *argv[])
{
    pthread_t tid;
    char buf[1024];
    int n;

    connect();
    if(pthread_create(&tid, NULL, readData, NULL) != 0) {
        printf("start read thread error");
        close(fd);
        return 0;
    }

    while(1) {
        n = read(STDIN_FILENO, buf, 1024);
        if(n < 0) {
            printf("read stdin error");
            continue;
        }

        write(fd, buf, n);
    }
    return 0;
}


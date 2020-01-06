
#include "anet.h"

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

typedef struct chatAeEvents {
    aeEventLoop *loop;
    int fd;
    int reading, writing;
} chatAeEvents;

static void chatAeReReadEvent(aeEventLoop *el, int fd, void *privdata, int mask) {
    
}

static void chatAeWriteEvent(aeEventLoop *el, int fd, void *privdata, int mask) {
    redisAeEvents *e = (redisAeEvents*)privdata;

}

static void chatAeAddRead(void *privData) {
   chatAeEvents *e = (chatAeEvents*)privData;
   aeEventLoop *loop = e->loop;
   if (!e->reading) {
       e->reading = 1;
       aeCreateFileEvent(loop, e->fd, AE_READABLE, cha)
   }
}

static void chatAeAddWrite(void privdata) {
    chatAeEvents *e = (chatAeEvents*)privdata;
    aeEventLoop *loop = e->loop;
    if(!e->writing) {
        e->writing = 1;
        aeCreateFileEvent(loop, e->fd, AE_WRITABLE,)
    }
}
int fd;
static void connect() {
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


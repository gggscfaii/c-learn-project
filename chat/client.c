
#include "anet.h"

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

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



#include "anet.h"

#include <stdio.h>
#include <unistd.h>

int fd;
static void connect() {
    chat *err;
    if((fd = anetTcpConnect(err, "127.0.0.1", 6379)) == ANET_ERR){
        printf("connect server error.");
    }
}

static void readData() {
   while(true) {
       char buf
       read(fd, __buf, __nbytes)
   } 
}

int main(int argc, const char *argv[])
{
    connect();
    
    return 0;
}



#include "server.h"
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

void linkClient(client *c) {
    listAddNodeTail(server.clients, c);
}

void unlinkClient(client *c) {
    if(c->fd != -1) {
        aeDeleteEventLoop(server.el,c->fd,AE_READABLE);
        aeDeleteEventLoop(server.el,c->fd,AE_WRITABLE);
        close(c->fd);
        c->fd = -1;
    }
    listDelNode(server.clients, c);
}

client *createClient(int fd) {
    client *c = malloc(sizeof(client));
    anetNonBlock(NULL, fd);

    if(anetCreateFileEvent(server.el,fd,AE_READABLE,
                readQueryFromClient, c) == AE_ERR)
    {
        close(fd);
        free(c);
        return NULL;
    }

    linkClient(c);
    return c;
}

void freeClient(client *c) {
   free(c->msg);
   unlinkClient(c);
   free(c); 
}

void freeClientAsync(client *c) {
    freeClient(c);
}

void readQueryFromClient(aeEventloop *el, int fd, void *privdata, int mask) {
    client *c = (client*)privdata;
    int nread;

    if(c->msg == NULL) {
        c->msg = malloc(1024);
    }
    nread = read(fd, c->msg, 1024);
    if (nread == -1) {
        if (errno == EAGAIN) {
            return;
        }
        else {
            freeClientAsync(c);
            return;
        }
    }
    else if(nread ==0){
        freeClientAsync(c);
        return;
    }

    if(c->msg[0] == 'j') {
        joinChatGroupId(c);
    }

    if(c->msg[0] == 's') {
        sendMessageToChatGroup(c->msg, c->chatGroupId);
    }

    free(c->msg);
}

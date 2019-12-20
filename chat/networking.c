
#include "server.h"
#include "list.h"
#include "anet.h"

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>

void linkClient(client *c) {
    listAddNodeTail(server.clients, c);
}

void unlinkClient(client *c) {
    if(c->fd != -1) {
        aeDeleteFileEvent(server.el,c->fd,AE_READABLE);
        aeDeleteFileEvent(server.el,c->fd,AE_WRITABLE);
        close(c->fd);
        c->fd = -1;
    }

    listNode *node = listSearchKey(server.clients, c);
    listDelNode(server.clients, node);
}

client *createClient(int fd) {
    client *c = (client*)malloc(sizeof(client));
    c->fd = fd;
    anetNonBlock(NULL, fd);

    if(aeCreateFileEvent(server.el,fd,AE_READABLE,
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

void readQueryFromClient(aeEventLoop *el, int fd, void *privdata, int mask) {
    client *c = (client*)privdata;
    int nread;

    if(c->msg == NULL) {
        c->msg = (char*)malloc(1024);
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
        joinChatGroupId(c, atoi(c->msg + 1));
    }

    if(c->msg[0] == 's') {
        sendMessageToChatGroup(c->msg, c->chatGroupId, nread);
    }

    free(c->msg);
    c->msg = NULL;
}

#define MAX_ACCEPTS_PER_CALL 1000
static void accpetCommonHandler(int fd, char *ip) {
    client *c;
    if ((c = createClient(fd)) == NULL) {
        close(fd);
    }
}

void acceptTcpHandler(aeEventLoop *el, int fd, void *privdata, int mask) {
    int cport, cfd, max = MAX_ACCEPTS_PER_CALL;
    char cip[NET_IP_STR_LEN];

    while (max--) {
        cfd = anetTcpAccept(server.neterr, fd, cip, sizeof(cip), &cport);
        if (cfd == ANET_ERR) {
            if(errno != EWOULDBLOCK);
            return;
        }
        accpetCommonHandler(cfd, cip);
    }
}

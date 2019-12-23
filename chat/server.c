

#include "server.h"
#include "list.h"
#include "anet.h"
#include "ae.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <stdlib.h>
/*=================================Globals===============================*/

/* Global vars */
struct chatServer server;

int listenToPort(int port, int *fds, int *count) {

    int unsupported = 0;
    *count = 0;
    fds[*count] = anetTcp6Server(server.neterr, port, NULL, 128);
    if(fds[*count] != ANET_ERR) {
        anetNonBlock(NULL, fds[*count]);
        (*count)++;
    }
    else if(errno == EAFNOSUPPORT) {
        unsupported++;
    }

    if (*count == 1 || unsupported) {
        fds[*count] = anetTcpServer(server.neterr, port, NULL, 128);
        if(fds[*count] != ANET_ERR) {
            anetNonBlock(NULL, fds[*count]);
            (*count)++;
        }
        else if(errno == EAFNOSUPPORT) {
            unsupported++;
        }
    }

    if(*count + unsupported == 2) return C_OK;
    return C_ERR;
}

void initServer() {
    server.clients = listCreate();
    server.el = aeCreateEventLoop(1024); 
    if(listenToPort(6379, server.ipfd, &server.ipfd_count) == C_ERR)
        exit(1);

    if(server.ipfd_count == 0) {
        exit(1);
    }

    for (int i = 0; i < server.ipfd_count; i++) {
        if(aeCreateFileEvent(server.el, server.ipfd[i], AE_READABLE,
                    acceptTcpHandler, NULL) == AE_ERR) {
            
        }
    }
}

void daemonsize(void) {
    int fd;

    if(fork() != 0) exit(0);
    setsid();

    if((fd = open("/dev/null", O_RDWR, 0)) != -1) {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        if(fd > STDERR_FILENO) close(fd);
    }
}

int main(int argc, const char *argv[])
{

    initServer();

    daemonsize();
    aeMain(server.el);
    aeStop(server.el);
    return 0;
}

void joinChatGroupId(client *c, int chatGroupId) {
    c->chatGroupId = chatGroupId;
}

void sendMessageToClient(client *c, char *msg, int len){
    int nwritten;
    nwritten = write(c->fd, msg, len);
    if(nwritten == -1) {
        if(errno == EAGAIN) {
            nwritten = 0;
        }
        else {
            freeClientAsync(c);
        }
    }
}

void sendMessageToChatGroup(char *msg, int chatGroupId, int len) {
    client *c;
    listNode *node;
    node = server.clients->head;
    while(node != NULL) {
        c = (client*)node->value;
        if(c->chatGroupId == chatGroupId)
            sendMessageToClient(c, msg, len);
        node = node->next; 
    }
}

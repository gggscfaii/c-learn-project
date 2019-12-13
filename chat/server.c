

#include "server.h"
#include "list.h"
#include "anet.h"
#include "ae.h"

#include <stdio.h>

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
    if(listenToPort(6379, server.ipfd, server.ipfd_count) == C_ERR)
        exit(1);

    if(server.ipfd_count == 0) {
        exit(1);
    }

    for (int i = 0; i < server.ipfd_count; i++) {
        if(aeCreateFileEvent(server.el, server.ipfd[i], AE_READABLE,
                    acceptTcpHandle, null) == AE_ERR) {
            
        }
    }
}

int main(int argc, const char *argv[])
{

    initServer();

    return 0;
}

static void acceptTcpHandle(aeEventLoop *eventLoop, int fd, void *clientData, int mask)
{
    
}

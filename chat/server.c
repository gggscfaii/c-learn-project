

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
    fds[*count] = anetTcp6Server(server.neterr, 6379, NULL, 128);
    if(fds[*count] != ANET_ERR) {
        anetNonBlock(NULL, fds[*count]);
        (*count)++;
    }
    else if(errno == EAFNOSUPPORT) {
        unsupported++;
    }

    if (*count == 1 || unsupported) {
        fds[*count] = anetTcpServer(server.neterr, 6379, NULL, 128);
        if(fds[*count] != ANET_ERR) {
            anetNonBlock(NULL, fds[*count]);
            (*count)++;
        }
        else if(errno == EAFNOSUPPORT) {
            unsupported++;
        }
    }

    
    if(fds[*count] == ANET_ERR) {
        return C_ERR;
    }

    return C_OK;
}

void initServer() {
    server.clients = listCreate();
}

int main(int argc, const char *argv[])
{

    initServer();

    return 0;
}


#ifndef __CHAT_H
#define __CHAT_H

#include <stdint.h>
#include "list.h"
#include "ae.h"

#define CONFIG_BINDADDR_MAX 16

typedef struct client {
    uint64_t id;         /* Client incrementtal unique ID.*/
    uint32_t chatGroupId;     /* chat group ID*/
    char *msg;  /*write msg*/
    int fd; /*file id*/
} client;

typedef struct chatDb {
    list *data;
    uint32_t chatGroupId;
} chatDb;

struct chatServer {
    list *clients;
    int ipfd[CONFIG_BINDADDR_MAX];
    int ipfd_count;
    char neterr[ANET_ERR_LEN];
    aeEventLoop el;
};

void acceptTcpHandle(aeEventLoop *eventLoop, int fd, void *clientData, int mask);
client *createClient(int fd);
void freeClient(client *c);
void freeClientAsync(client *c);
void readQueryFromClient(aeEventloop *el, int fd, void *privdata, int mask);
void joinChatGroupId(client *c);
void sendMessageToClient(client *c, char *msg);
void sendMessageToChatGroup(char *msg, int chatGroupId);

#endif

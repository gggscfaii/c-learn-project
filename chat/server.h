#ifndef __CHAT_H
#define __CHAT_H

#include <stdint.h>
#include "list.h"
#include "ae.h"
#include "anet.h"

#define CONFIG_BINDADDR_MAX 16
#define NET_IP_STR_LEN 46

#define C_OK 0
#define C_ERR 1
typedef struct client {
    uint64_t id;         /* Client incrementtal unique ID.*/
    int chatGroupId;     /* chat group ID*/
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
    aeEventLoop *el;
};

extern struct chatServer server;

void acceptTcpHandler(aeEventLoop *el, int fd, void *clientData, int mask);
client *createClient(int fd);
void freeClient(client *c);
void freeClientAsync(client *c);
void readQueryFromClient(aeEventLoop *el, int fd, void *privdata, int mask);
void joinChatGroupId(client *c, int chatGroupId);
void sendMessageToClient(client *c, char *msg, int len);
void sendMessageToChatGroup(char *msg, int chatGroupId, int len);

#endif

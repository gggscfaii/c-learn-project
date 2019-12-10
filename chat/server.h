#ifndef __CHAT_H
#define __CHAT_H

#include <stdint.h>
#include "list.h"
#include "ae.h"

typedef struct client {
    uint64_t id;         /* Client incrementtal unique ID.*/
    uint32_t chatGroupId;     /* chat group ID*/
    char *msg;  /*write msg*/
} client;

typedef struct chatDb {
    list *data;
    uint32_t chatGroupId;
} chatDb;

struct chatServer {
    list *clients;
};

void sendMessageToClient(client *c, char *msg);
void sendMessageToChatGroup(char *msg, int chatGroupId);

#endif

#ifndef __CHAT_H
#define __CHAT_H

#include "list.h"

typedef struct client {
    uint64_t id;         /* Client incrementtal unique ID.*/
    unsigned int chatGroupId;     /* chat group ID*/
} client;

typedef struct chatDb {
    list *clients;
} chatDb;

void sendMessageToClient(client *c, char *msg);
void sendMessageToChatGroup(client *c, char *msg, int chatGroupId);


#endif

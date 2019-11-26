#ifndef __CHAT_H
#define __CHAT_H

typedef struct client {
    unit64_t id;         /* Client incrementtal unique ID.*/
    int chatGroupId;     /* chat group ID*/
} client;

typedef struct chatDb {
    list *clients;
}
#endif

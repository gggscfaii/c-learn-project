#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <malloc.h>

#include "ae.h"
#include "ae_select.c"


aeEventLoop *aeCreateEventLoop(int setsize) {
    aeEventLoop *eventLoop;
    int i;

    if((eventLoop = malloc(sizeof(*eventLoop))) == NULL) goto err;
    eventLoop->evnets = malloc(sizeof(aeFileEvent)*setsize);
    eventLoop->fired = malloc(sizeof(aeFiredEvent)*setsize);
    if(eventLoop->fired == NULL) goto err;
    eventLoop->maxfd = -1;
    eventLoop->setsize = setsize;
    if(aeApiCreate(eventLoop) == -1) goto err;
    for (int i = 0; i < setsize; i++) {
        eventLoop->events[i].mask = AE_NONE;
    }
err:
    if(eventLoop) {
        zfree(eventLoop->fired);
        zfree(eventLoop);
    }
    return NULL;
}


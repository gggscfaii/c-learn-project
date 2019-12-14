
#include <sys/select.h>
#include <string.h>

typedef struct aeApiState {
    fd_set rfds, wfds;
    fd_set _rfds, _wfds;
} aeApiState;

static int aeApiCreate(aeEventLoop *eventLoop) {
    aeApiState *state = malloc(sizeof(aeApiState));

    if(!state) return -1;
    FD_ZERO(&state->rfds);
    FD_ZERO(&state->wfds);
    eventLoop->apidata = state;
    return 0; 
}

static void aeApiFree(aeEventLoop *eventLoop) {
    free(eventLoop->apidata);
}

static int aeApiAddEvent(aeEventLoop *eventLoop, int fd, int mask) {
    aeApiState *state = eventLoop->apidata;
    if(mask & AE_READABLE) FD_SET(fd, &state->rfds);
    if(mask & AE_WRITABLE) FD_SET(fd, &state->wfds);
    return 0; 
}

static void aeApiDelEvent(aeEventLoop *eventLoop, int fd, int mask) {
    aeApiState *state = eventLoop->apidata;
    if(mask & AE_READABLE) FD_CLR(fd, &state->rfds);
    if(mask & AE_WRITABLE) FD_CLR(fd, &state->wfds);
}

static int aeApiPoll(aeEventLoop *eventLoop, struct timeval *tvp) {
    aeApiState *state = eventLoop->apidata;
    int retval, numevents = 0;

    memcpy(&state->_rfds, &state->rfds, sizeof(fd_set));
    memcpy(&state->_wfds, &state->wfds, sizeof(fd_set));

    retval = select(eventLoop->maxfd+1,
            &state->_rfds, &state->_wfds, NULL, tvp);
    if(retval > 0) {
        for (int i = 0; i <= eventLoop->maxfd; i++) {
            int mask = 0;
            aeFileEvent *fe = &eventLoop->events[i];
            
            if(fe->mask == AE_NONE) continue;
            if(fe->mask & AE_READABLE && FD_ISSET(i, &state->_rfds))
                mask |= AE_READABLE;
            if(fe->mask & AE_WRITABLE && FD_ISSET(i, &state->_wfds))
                mask |= AE_WRITABLE;
            eventLoop->fired[numevents].fd = i;
            eventLoop->fired[numevents].mask = mask;
            numevents++;   
        }
    }
    return numevents;
}

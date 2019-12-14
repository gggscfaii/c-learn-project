/** anet.c Basic Tcp socket*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>

#include "anet.h"

static void anetSetError(char *err, const char *fmt, ...)
{
    va_list ap;
    if(!err) return;
    va_start(ap, fmt);
    vsnprintf(err, ANET_ERR_LEN, fmt, ap);
    va_end(ap);
}

int anetSetBlock(char *err, int fd, int non_block){
    int flags;

    /* set the socket bloking (if non_block is zero) or non-bloking.
     * Not that fcntl(2) for F_GETFL and F_SETFL can't be
     * interrupted by a signal.*/
    if((flags = fcntl(fd, F_GETFL)) == -1){
        anetSetError(err, "fcntl(F_GETFL): %s", strerror(errno));
        return ANET_ERR;
    }

    if(non_block)
        flags |= O_NONBLOCK;
    else
        flags &= ~O_NONBLOCK;

    if (fcntl(fd, F_SETFL, flags) == -1) {
        anetSetError(err, "fcntl(F_SETFL,O_NONBLOCK): %s", strerror(errno));
        return ANET_ERR;
    }
    return ANET_OK;
}

int anetNonBlock(char *err, int fd){
    return anetSetBlock(err,fd,1);
}

int anetBlock(char *err, int fd){
    return anetSetBlock(err,fd,0);
}

static int anetSetReuseAddr(char *err, int fd){
    int yes = 1;
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1){
        anetSetError(err, "setsockopt SO_REUSEADDR: %s", strerror(errno));
        return ANET_ERR;
    }
    return ANET_OK;
}

#define ANET_CONNECT_NONE 0
#define ANET_CONNECT_NONBLOCK 1
#define ANET_CONNECT_BE_BINDING 2
static int anetTcpGenericConnect(char *err, const char *addr, int port,
        const char *source_addr, int flags)
{
    int s = ANET_ERR, rv;
    char portstr[6]; /*strlen("65535") + 1; */
    struct addrinfo hints, *servinfo, *bservinfo, *p, *b;

    snprintf(portstr, sizeof(portstr), "%d", port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(addr,portstr,&hints,&servinfo) != 0)) {
        anetSetError(err, "%s", gai_strerror(rv));
        return ANET_ERR;
    }
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if((s = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;
        if(anetSetReuseAddr(err,s) == ANET_ERR) goto error;
        if(flags & ANET_CONNECT_NONBLOCK && anetNonBlock(err,s) != ANET_OK)
            goto error;
        if(source_addr){
            int bound = 0;
            if ((rv = getaddrinfo(source_addr, NULL, &hints, &bservinfo)) != 0) {
                anetSetError(err, "%s", gai_strerror(rv));
                goto error;
            }

            for(b = bservinfo; b !=NULL; b = b->ai_next){
                if(bind(s,b->ai_addr,b->ai_addrlen) != -1){
                    bound = 1;
                    break;
                }
            }
            freeaddrinfo(bservinfo);
            if(!bound){
                anetSetError(err, "bind: %s", strerror(errno));
                goto error;
            }
        }

        if (connect(s,p->ai_addr,p->ai_addrlen) == -1) {
            if(errno == EINPROGRESS && flags & ANET_CONNECT_NONBLOCK)
                goto end;
            close(s);
            s = ANET_ERR;
            continue;
        }

        goto end;
    }

    if(p == NULL)
        anetSetError(err, "creating socket: %s", strerror(errno));

error:
    if(s != ANET_ERR){
        close(s);
        s = ANET_ERR;
    }
end: 
    freeaddrinfo(servinfo);

    if(s == ANET_ERR && source_addr && (flags & ANET_CONNECT_BE_BINDING)){
        return anetTcpGenericConnect(err,addr,port, NULL,flags);
    }
    else{
        return s;
    }
}

int anetTcpConnect(char *err, const char *addr, int port)
{
    return anetTcpGenericConnect(err,addr,port,NULL,ANET_CONNECT_NONE);
}

int anetTcpNonBlockConnect(char *err, const char *addr, int port)
{
    return anetTcpGenericConnect(err,addr,port,NULL,ANET_CONNECT_NONBLOCK);
}

int anetTcpNonBlockBindConnect(char *err, const char *addr, int port,
                               const char *source_addr)
{
    return anetTcpGenericConnect(err,addr,port,source_addr,
            ANET_CONNECT_NONBLOCK);
}

int anetTcpNonBlockBestEfforBindConnect(char *err, const char *addr, int port,
                                        const char *source_addr)
{
    return anetTcpGenericConnect(err,addr,port,source_addr,
            ANET_CONNECT_NONBLOCK|ANET_CONNECT_BE_BINDING);
}

static int anetV6Only(char *err, int s){
    int yes = 1;
    if(setsockopt(s,IPPROTO_IPV6,IPV6_V6ONLY,&yes,sizeof(yes)) == -1){
        anetSetError(err, "set sockopt: %s", strerror(errno));
        close(s);
        return ANET_ERR;
    }
    return ANET_OK;
}

static int anetListen(char *err, int s, struct sockaddr *sa, socklen_t len, int backlog) {
    if (bind(s,sa,len) == -1) {
        anetSetError(err, "bind:%s", strerror(errno));
        close(s);
        return ANET_ERR;
    }

    if (listen(s,backlog) == -1) {
        anetSetError(err, "Listen: %s", strerror(errno));
        close(s);
        return ANET_ERR;        
    }
    return ANET_OK;
}

static int _anetTcpServer(char *err, int port, char *bindaddr, int af, int backlog)
{
    int s = -1, rv;
    char _port[6];
    struct addrinfo hints, *servinfo, *p;

    snprintf(_port,6,"%d",port);
    memset(&hints,0,sizeof(hints));
    hints.ai_family = af;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if((rv = getaddrinfo(bindaddr,_port, &hints,&servinfo)) != 0){
        anetSetError(err, "%s", gai_strerror(rv));
        return ANET_ERR;
    }
    for(p = servinfo; p != NULL; p = p->ai_next){
        if((s = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == -1)
            continue;

        if(af == AF_INET6 && anetV6Only(err,s) == ANET_ERR) goto error;
        if(anetSetReuseAddr(err,s) == ANET_ERR) goto error;
        if(anetListen(err,s,p->ai_addr,p->ai_addrlen,backlog) == ANET_ERR) s = ANET_ERR;
        goto end;
    }
    if(p == NULL){
        anetSetError(err, "unable to bind socket, errno: %d", errno);
        goto error;
    }

error:
    if(s != -1) close(s);
    s = ANET_ERR;
end:
    freeaddrinfo(servinfo);
    return s;
}

int anetTcpServer(char *err, int port, char *bindaddr, int backlog)
{
    return _anetTcpServer(err, port, bindaddr, AF_INET, backlog);
}

int anetTcp6Server(char *err, int port, char *bindaddr, int backlog)
{
    return _anetTcpServer(err, port, bindaddr, AF_INET6, backlog);
}

static int anetGenericAccept(char *err, int s, struct sockaddr *sa, socklen_t *len) {
    int fd;
    while(1) {
        fd = accept(s,sa,len);
        if(fd == -1) {
            if(errno == EINTR)
                continue;
            else {
                anetSetError(err, "accept: %s", strerror(errno));
                return ANET_ERR;
            }
        }
        break;
    }
    return fd;
}

int anetTcpAccept(char *err, int s, char *ip, size_t ip_len, int *port) {
    int fd;
    struct sockaddr_storage sa;
    socklen_t salen = sizeof(sa);
    if((fd = anetGenericAccept(err,s,(struct sockaddr*)&sa,&salen)) == -1)
        return ANET_ERR;

    if (sa.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&sa;
        if (ip) inet_ntop(AF_INET, (void*)&(s->sin_addr), ip, ip_len);
        if (port) *port = ntohs(s->sin_port);
    }
    else {
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&sa;
        if (ip) inet_ntop(AF_INET6, (void*)&(s->sin6_addr),ip,ip_len);
        if (port) *port = ntohs(s->sin6_port);
    }
    return fd;
}

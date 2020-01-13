/*anet.c Basic Tcp socket stuff made a less boring
 * sdwang
 * 2019-11-26 09:49
 * */

#ifndef ANET_H
#define ANET_H

#include <sys/types.h>
#include <sys/socket.h>

#define ANET_OK 0
#define ANET_ERR -1
#define ANET_ERR_LEN 256

/*Flags used with certin function*/
#define ANET_NONE 0
#define ANET_IP_ONLY (1<<0)

int anetTcpConnect(char *err, const char *addr, int port, struct sockaddr *ret_addr, size_t *ret_len);
int anetTcpNonBlockConnect(char *err, const char *addr, int port, struct sockaddr *ret_addr, size_t *ret_len);
int anetTcpNonBlockBindConnect(char *err, const char *addr, int port, const char *source_addr, struct sockaddr *ret_addr, size_t *ret_len);
int anetTcpNonBlockBestEffortBindConnect(char *err, const char *addr, int port, const char *source_addr, struct sockaddr *ret_addr, size_t *ret_len);
int anetRead(int fd, char *buf, int count);
int anetWrite(int fd, char *buf, int count);
int anetNonBlock(char *err, int fd);
int anetBlock(char *err, int fd);
int anetTcpServer(char *err, int port, char *bindaddr, int backlog);
int anetTcp6Server(char *err, int port, char *bindaddr, int backlog);
int anetTcpAccept(char *err, int s, char *ip, size_t ip_len, int *port);
#endif

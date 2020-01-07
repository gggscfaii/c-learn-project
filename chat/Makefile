# chat Makefile

CHAT_SERVER_NAME=chat-server
CHAT_CLIENT_NAME=chat-client
CHAT_SERVER_OBJ=list.o ae.o anet.o server.o networking.o
CHAT_CLIENT_OBJ=ae.o anet.o client.o
CFLAGS+=-g
all:${CHAT_SERVER_NAME} ${CHAT_CLIENT_NAME}
.PHONY:all

${CHAT_SERVER_NAME}: ${CHAT_SERVER_OBJ}
	$(CC) -g  -o $@ $^
${CHAT_CLIENT_NAME}: ${CHAT_CLIENT_OBJ}
	$(CC) -g  -o $@ $^ -lpthread
clean:
	rm -rf ${CHAT_SERVER_OBJ} ${CHAT_SERVER_NAME} ${CHAT_CLIENT_OBJ} ${CHAT_CLIENT_NAME}

.PHONY: clean

install: all

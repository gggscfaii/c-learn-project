# chat Makefile

CHAT_SERVER_NAME=chat-server
CHAT_SERVER_OBJ=list.o ae.o anet.o server.o networking.o
CFLAGS+=-g
${CHAT_SERVER_NAME}: ${CHAT_SERVER_OBJ}
	$(CC) -g  -o $@ $^
.PHONY: all

clean:
	rm -rf ${CHAT_SERVER_OBJ} ${CHAT_SERVER_NAME}

.PHONY: clean

install: all

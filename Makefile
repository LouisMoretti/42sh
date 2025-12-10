CC=gcc
CFLAGS=-std=c99 -pedantic -Werror -Wall -Wextra -Wvla
CPPFLAGS=
LDFLAGS=
LDLIBS=

SRCS=src/main.c
OBJS=${SRCS:.c=.o}
BIN=42sh

${BIN}: ${OBJS}
	${CC} ${LDLIBS} ${LDFLAGS} $^ -o $@

${OBJS}: ${SRCS}

.PHONY: clean check run

run:
	./42sh

check:
	echo "Here will be the future testsuite..."

clean:
	${RM} ${BIN} ${OBJS}

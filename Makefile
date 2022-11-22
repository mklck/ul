.POSIX:

SRC = main.c parser.c misc.c ds.c eval.c core.c
OBJ = ${SRC:.c=.o}
BIN_NAME = ul

all: ${BIN_NAME}

${BIN_NAME}: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f ${BIN_NAME} ${OBJ}

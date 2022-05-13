#? @authors: Esmaail Albarazi

CC=gcc
FLAGS=-Wall -pthread
PROG=projeto
OBJS=projeto.o

all:	${PROG}

clean:
			rm	${OBJS} *~ ${PROG}

${PROG}:	${OBJS}
			${CC} ${FLAGS} ${OBJS} -lm -o $@

.c.o:
			${CC} ${FLAGS} $< -c -o $@

################################################################
projeto.o: projeto.c

netINC= -I/usr/include
LIB= -L/usr/lib
CFLAGS=-Wall -g ${INC} -L. ${LIB}
CC=gcc

operador.o: operador.c ep1.h
	$(CC) $(CFLAGS) -c $*.c

operador : operador.o
	${CC} ${CFLAGS} -o operador operador.o

supervisor.o: supervisor.c ep1.h
	$(CC) $(CFLAGS) -c $*.c

supervisor : supervisor.o
	${CC} ${CFLAGS} -o supervisor supervisor.o

clean: 
	rm -f *.o operador supervisor *~
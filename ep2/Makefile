netINC= -I/usr/include
LIB= -L/usr/lib
CFLAGS=-Wall -g ${INC} -L. ${LIB}
CC=gcc

ep2a.o: ep2a.c
	$(CC) $(CFLAGS) -c $*.c

ep2a : ep2a.o
	${CC} ${CFLAGS} -o ep2a ep2a.o -lpthread

ep2b.o: ep2b.c
	$(CC) $(CFLAGS) -c $*.c

ep2b : ep2b.o
	${CC} ${CFLAGS} -o ep2b ep2b.o -lpthread

clean:
	rm -f *.o ep2a ep2b *~
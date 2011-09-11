CC = gcc
CFLAGS = -std=gnu99 -ggdb
LIBS =

all: myshell

OBJS = myshell.o

myshell: myshell.o
	$(CC) $(CFLAGS) -o myshell $(OBJS) $(LIBS)

clean:
	rm *.o
	rm myshell

.c.o:
	$(CC) -c $(CFLAGS) $*.c

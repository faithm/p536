CC = gcc
CFLAGS = -std=gnu99 -ggdb
LIBS =

all: myshell

OBJS = myshell.o safe_funcs.o pipe.o

myshell: myshell.o safe_funcs.o pipe.o
	$(CC) $(CFLAGS) -o myshell $(OBJS) $(LIBS)

clean:
	rm *.o
	rm myshell

.c.o:
	$(CC) -c $(CFLAGS) $*.c

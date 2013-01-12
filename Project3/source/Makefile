CC=cc
CFLAGS=-Wall

all: cdDemo envDemo1 envDemo2 simpleShell

cdDemo: cdDemo.c
	$(CC) $(CFLAGS) cdDemo.c -o cdDemo

envDemo1: envDemo1.c
	$(CC) $(CFLAGS) envDemo1.c -o envDemo1

envDemo2: envDemo2.c
	$(CC) $(CFLAGS) envDemo2.c -o envDemo2

simpleShell: simpleShell.c
	$(CC) $(CFLAGS) simpleShell.c -o simpleShell

clean:
	rm cdDemo envDemo1 envDemo2 simpleShell

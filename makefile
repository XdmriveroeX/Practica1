CC = gcc
CFLAGS = -Wall -Wextra

all: interface searcher p1-odProgram

interface: interface.c
	$(CC) $(CFLAGS) -o interface interface.c

searcher: searcher.c
	$(CC) $(CFLAGS) -o searcher searcher.c

p1-odProgram: p1-odProgram.c
	$(CC) $(CFLAGS) -o p1-odProgram p1-odProgram.c

clean:
	rm -f interface searcher p1-odProgram

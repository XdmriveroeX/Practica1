CC = gcc
CFLAGS = -Wall -Wextra

all: interface searcher p1-odProgram create_indexed_file

interface: interface.c
	$(CC) $(CFLAGS) -o interface interface.c

searcher: searcher.c
	$(CC) $(CFLAGS) -o searcher searcher.c

p1-odProgram: p1-odProgram.c
	$(CC) $(CFLAGS) -o p1-odProgram p1-odProgram.c

create_indexed_file: create_indexed_file.c
	$(CC) $(CFLAGS) -o create_indexed_file create_indexed_file.c

clean:
	rm -f interface searcher p1-odProgram create_indexed_file

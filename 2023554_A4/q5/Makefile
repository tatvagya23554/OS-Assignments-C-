CC = gcc
CFLAGS = -Wall -Wextra -O2

all: list countw copy move main

list: list.c
	$(CC) $(CFLAGS) -o list list.c
countw: countw.c
	$(CC) $(CFLAGS) -o countw countw.c
copy: copy.c
	$(CC) $(CFLAGS) -o copy copy.c
move: move.c
	$(CC) $(CFLAGS) -o move move.c
main: main.c list countw copy move
	$(CC) $(CFLAGS) -o main main.c

clean: 
	rm -f list countw copy move main
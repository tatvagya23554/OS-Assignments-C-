CC = gcc
CFLAGS = -Wall -Wextra -O2

all: date cal uptime main

date: date.c
	$(CC) $(CFLAGS) -o date date.c
cal: cal.c
	$(CC) $(CFLAGS) -o cal cal.c
uptime: uptime.c
	$(CC) $(CFLAGS) -o uptime uptime.c
main: main.c date cal uptime
	$(CC) $(CFLAGS) -o main main.c

clean:
	rm -f date cal uptime main
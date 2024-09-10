CC = gcc

CFLAGS = -Wall -Werror -pedantic -O2

all: client server

client: client.c
	$(CC) $(CFLAGS) -o client client.c

server: movie_server.c
	$(CC) $(CFLAGS) -o movie_server movie_server.c

clean:
	rm -rf client movie_server

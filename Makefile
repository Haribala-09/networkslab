all: server client

server:	
	gcc -g -O2 serverupd.c -o serverupd

client:
	gcc -g -O2 clientupd.c -o clientupd
clean:
	rm clientupd serverupd
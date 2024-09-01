all: server client

server:	
	gcc -g -O2 -Wall -Wextra -pedantic serverupd.c -o serverupd 

client:
	gcc -g -O2 -Wall -Wextra -pedantic clientupd.c -o clientupd
clean:
	rm clientupd serverupd
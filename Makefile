all: client server

server:
	gcc -g -pthread server.c -o server

client:
	gcc -g -lrt client.c client_lib.c -o client

clean:
	rm -f client server

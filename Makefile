all: client server

server:
	gcc -pthread server.c -o server

client:
	gcc -lrt client.c client_lib.c -o client

clean:
	rm -f client server

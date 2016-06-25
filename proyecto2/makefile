all: server client

circular_buffer.o: circular_buffer.c
	gcc -c circular_buffer.c

server: server.c circular_buffer.o
	gcc server.c -pthread circular_buffer.o -o server

client: client.c 
	gcc client.c -o client

clean:
	rm -rf *.o server client
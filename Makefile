all:client server
client:client.o
	gcc client.o -o client -lpthread
server:server.o
	gcc server.o -o server -lpthread

server.o:server.c
	gcc -c server.c
client.o:client.c
	gcc -c client.c
	


.PHONY:clean
clean:
	rm -f *.o
	rm -f server
	rm -f client
	rm -f *.db

 
clear:
	rm *.db

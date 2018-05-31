CC= gcc

project: server client

server: server.o utils.o inventory.o
		$(CC) -o server server.o utils.o inventory.o
client: client.o
		$(CC) -o client client.o

server.o:
		$(CC) -c server.c
utils.o:
		$(CC) -c utils.c 
inventory.o:
		$(CC) -c inventory.c	
client.o:
		$(CC) -c client.c

clean:
	./rm_all_obj_and_oldversion_file.sh && ./myls.sh
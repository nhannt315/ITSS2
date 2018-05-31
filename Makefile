CC= gcc
FLAGS=  -pthread

project: server client

server: server.o utils.o inventory.o
		$(CC) $(FLAGS) -o server server.o utils.o inventory.o
client: client.o
		$(CC) $(FLAGS) -o client client.o

server.o:
		$(CC) $(FLAGS) -c server.c
utils.o:
		$(CC) $(FLAGS) -c utils.c 
inventory.o:
		$(CC) $(FLAGS) -c inventory.c	
client.o:
		$(CC) $(FLAGS) -c client.c

clean:
	./rm_all_obj_and_oldversion_file.sh && ./myls.sh
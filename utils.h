#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int compareIP(struct sockaddr_in *addr1, struct sockaddr_in *addr2);
int comparePort(struct sockaddr_in *addr1, struct sockaddr_in *addr2);
int compareSockaddr(struct sockaddr_in *addr1, struct sockaddr_in *addr2);
char *getIpFromSockAddr(struct sockaddr_in *addr);
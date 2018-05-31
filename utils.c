#include "utils.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int compareIP(struct sockaddr_in *addr1, struct sockaddr_in *addr2) {
  struct in_addr ipAddr1 = addr1->sin_addr;
  struct in_addr ipAddr2 = addr2->sin_addr;
  char ip1[INET_ADDRSTRLEN], ip2[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &ipAddr1, ip1, INET_ADDRSTRLEN);
  inet_ntop(AF_INET, &ipAddr2, ip2, INET_ADDRSTRLEN);
  return strcmp(ip1, ip2);
}

int comparePort(struct sockaddr_in *addr1, struct sockaddr_in *addr2) {
  int port1 = ntohs(addr1->sin_port);
  int port2 = ntohs(addr2->sin_port);
  return (port1 == port2) ? 1 : 0;
}

int compareSockaddr(struct sockaddr_in *addr1, struct sockaddr_in *addr2) {
  if (addr1 == NULL || addr2 == NULL) {
    return -1;
  }
  if (compareIP(addr1, addr2) == 0 && comparePort(addr1, addr2) == 1) {
    return 1;
  }
  return 0;
}

char *getIpFromSockAddr(struct sockaddr_in *addr) {
  struct in_addr ipAddr1 = addr->sin_addr;
  char *ip1 = malloc(INET_ADDRSTRLEN);
  inet_ntop(AF_INET, &ipAddr1, ip1, INET_ADDRSTRLEN);
  return ip1;
}
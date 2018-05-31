#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "constant.h"
#include "inventory.h"
#include "utils.h"

#define MAXBUFSIZE 1024
#define MAXPENDING 10

#define DEFAULTQUANTITY 10
#define SALE_OPERATION 1
#define CHECK_OPERATION 0
#define NEED_DELIVERY 3

void handleClient(int clientSocket, struct sockaddr_in *clientAddr);
void salesMng(int clientSock, struct sockaddr_in *clientAddr,
              char *receivedContent);
void initInventory();
int checkAndAndAddNewVd(int clientSock, struct sockaddr_in *addr);
void sendInventoryInfo(int clientSock, struct sockaddr_in *addr);
int getMachineNumber(struct sockaddr_in *addr);
int equipInfoAccess(int flag, int clientSock, struct sockaddr_in *clientAddr,
                    char *selection);
void connectMng(int argc, char **argv);
void delivery();

int main(int argc, char **argv) {
  pid_t deliveryPid, connectPid;
  int status;
  initInventoryMemory();
  initInventory();
  connectPid = fork();
  if (connectPid == 0) {
    deliveryPid = fork();
    int status2;
    if (deliveryPid == 0) {
      while (1) {
        delivery();
        sleep(10);
      }
    } else {
      connectMng(argc, argv);
    }
  } else {
    wait(&status);
  }
}

void delivery() {
  printf("Start delivery checking!\n");
  Inventory *inventory = getInventory();
  printf("Hihi : %d\n", inventory->vdList[0].stingQuantity);
}

void connectMng(int argc, char **argv) {
  int servSock;
  int clntSock;
  struct sockaddr_in servAddr;
  struct sockaddr_in clntAddr;
  unsigned short servPort;
  unsigned int clntLen;
  char sendBuffer[MAXBUFSIZE];
  pid_t processID;
  unsigned int childProcCount = 0;
  // initInventory();

  if (argc != 2) {
    fprintf(stderr, "UseAge: %s <Server Port>\n", argv[0]);
    exit(1);
  }
  servPort = atoi(argv[1]);
  if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    perror("socket() failed");
    exit(1);
  }
  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddr.sin_port = htons(servPort);
  if (bind(servSock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
    perror("bind() failed");
    exit(1);
  }
  printf("Server running at port %s\n", argv[1]);
  if (listen(servSock, MAXPENDING) < 0) {
    perror("listen() failed");
    exit(1);
  }
  printf("Waiting for incoming connections!\n");

  while (1) {
    clntLen = sizeof(clntAddr);
    if ((clntSock = accept(servSock, (struct sockaddr *)&clntAddr, &clntLen)) <
        0) {
      perror("accept() failed");
    }
    if (checkAndAndAddNewVd(clntSock, &clntAddr) == 0) {
      perror("No more slot for this machine, get away!\n");
      send(clntSock, "401", 4, 0);
      close(clntSock);
      continue;
    } else {
      printf("check send inventory\n");
      sendInventoryInfo(clntSock, &clntAddr);
    }
    if ((processID = fork()) < 0) {
      perror("Accept() failed");
      exit(1);
    } else if (processID == 0) {
      close(servSock);
      printf("Handling client %s\n", inet_ntoa(clntAddr.sin_addr));
      // Handle
      printf("Handle client!\n");
      handleClient(clntSock, &clntAddr);
      exit(0);
    }
  }
}

int equipInfoAccess(int flag, int clientSock, struct sockaddr_in *clientAddr,
                    char *selection) {
  Inventory *inventory = getInventory();
  if (flag == SALE_OPERATION) {
    time_t rawtime;
    struct tm *info;
    time(&rawtime);
    info = localtime(&rawtime);
    FILE *fout;
    fout = fopen("log.txt", "a");
    fprintf(fout, "%s : %s %s \n", getIpFromSockAddr(clientAddr), asctime(info),
            selection);
    fclose(fout);
    int i = getMachineNumber(clientAddr);
    if (strcmp(selection, "sting") == 0) {
      inventory->vdList[i].stingQuantity--;
    } else if (strcmp(selection, "nutri") == 0) {
      inventory->vdList[i].nutriQuantity--;
    } else if (strcmp(selection, "monster") == 0) {
      inventory->vdList[i].monsterQuantity--;
    }
    printf("sting:%d nutri:%d monster:%d\n", inventory->vdList[i].stingQuantity,
           inventory->vdList[i].nutriQuantity,
           inventory->vdList[i].monsterQuantity);
    char *message = malloc(100);
    sprintf(message, "%d|%d|%d|%d", QUANTITY_MESSAGE_CODE,
            inventory->vdList[i].stingQuantity,
            inventory->vdList[i].nutriQuantity,
            inventory->vdList[i].monsterQuantity);
    
    send(clientSock, message, 100, 0);
    saveInventory(inventory);
  }
}

void sendInventoryInfo(int clientSock, struct sockaddr_in *addr) {
  int i = getMachineNumber(addr);
  Inventory *inventory = getInventory();
  char *message = malloc(100);
  sprintf(message, "%d|%d|%d|%d", QUANTITY_MESSAGE_CODE,
          inventory->vdList[i].stingQuantity,
          inventory->vdList[i].nutriQuantity,
          inventory->vdList[i].monsterQuantity);
  send(clientSock, message, 100, 0);
  free(inventory);
}

void handleClient(int clientSock, struct sockaddr_in *clientAddr) {
  char buffer[MAXBUFSIZE];
  int recvMsgSize;

  if ((recvMsgSize = recv(clientSock, buffer, MAXBUFSIZE, 0)) < 0) {
    perror("receive failed!\n");
    exit(1);
  }
  while (recvMsgSize > 0) {
    buffer[recvMsgSize] = '\0';
    salesMng(clientSock, clientAddr, buffer);
    if ((recvMsgSize = recv(clientSock, buffer, MAXBUFSIZE, 0)) < 0) {
      perror("recv() failed");
      exit(1);
    }
  }
}

void salesMng(int clientSock, struct sockaddr_in *clientAddr,
              char *receivedContent) {
  pid_t childPid, wpid;
  int status;
  switch (childPid = fork()) {
    case -1:
      perror("Error when generate child process!\n");
      exit(1);
    case 0:
      equipInfoAccess(SALE_OPERATION, clientSock, clientAddr, receivedContent);
      break;
    default:
      wpid = wait(&status);
      break;
  }
}

void initInventory() {
  int i;
  Inventory *newInventory = malloc(sizeof(Inventory));
  for (i = 0; i < MAXCLIENT; i++) {
    newInventory->vdList[i].isConnected = 0;
    newInventory->vdList[i].monsterQuantity = DEFAULTQUANTITY;
    newInventory->vdList[i].nutriQuantity = DEFAULTQUANTITY;
    newInventory->vdList[i].stingQuantity = DEFAULTQUANTITY;
  }
  saveInventory(newInventory);
}

int checkAndAndAddNewVd(int clientSock, struct sockaddr_in *addr) {
  int i;
  Inventory *inventory = getInventory();
  for (i = 0; i < MAXCLIENT; i++) {
    if (inventory->vdList[i].ip != NULL &&
        inventory->vdList[i].isConnected == 0) {
      if (strcmp(inventory->vdList[i].ip, getIpFromSockAddr(addr)) == 0) {
        inventory->vdList[i].isConnected = 1;
        saveInventory(inventory);
        return 1;
      }
    } else if (inventory->vdList[i].isConnected == 0 &&
               inventory->vdList[i].ip == NULL) {
      inventory->vdList[i].ip = getIpFromSockAddr(addr);
      inventory->vdList[i].clientSock = clientSock;
      inventory->vdList[i].isConnected = 1;
      saveInventory(inventory);
      return 1;
    }
  }
  saveInventory(inventory);
  return 0;
}

int getMachineNumber(struct sockaddr_in *addr) {
  int i;
  Inventory *inventory = getInventory();
  for (i = 0; i < MAXCLIENT; i++) {
    if (strcmp(inventory->vdList[i].ip, getIpFromSockAddr(addr)) == 0) {
      return i;
    }
  }
  return -1;
}
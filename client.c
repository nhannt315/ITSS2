#include <arpa/inet.h>
#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include "constant.h"

#define maxbuf_size 1024
#define nbq 10
#define PROC_OK 0
#define PROC_NG -1

typedef struct {
  int stingQuantity;
  int nutriQuantity;
  int monsterQuantity;
} VendingMachine;

VendingMachine thisMachine;

void equipMain(int sock);
int showMenu();
void commoditySales(char *selection, int sock);
void handleMessage(char *message);

int main(int argc, char *argv[]) {
  int sock;
  struct sockaddr_in servAddr;
  unsigned short servPort;
  char *servIP;

  /*-----------menu--------------*/

  if (argc != 3) {
    fprintf(stderr, "Usage: %s <Server IP> <Echo Port>\n", argv[0]);
    exit(1);
  }

  servIP = argv[1];
  servPort = atoi(argv[2]);

  if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    perror("socket() failed");
    exit(1);
  }

  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = inet_addr(servIP);
  servAddr.sin_port = htons(servPort);

  if (connect(sock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
    perror("connect() failed");
    exit(1);
  }

  char message[100];

  if (recv(sock, message, 100, 0) == 0) {
    // error: server terminated prematurely
    perror("The server terminated prematurely");
    exit(4);
  }

  handleMessage(message);

  equipMain(sock);

  return 0;
}

void equipMain(int sock) {
  char message[100];
  int beingUsed = 1;
  while (beingUsed) {
    int choice = showMenu();
    char selection[100];
    switch (choice) {
      case 1:
        if (thisMachine.stingQuantity == 0) {
          printf("Out of stock, go buy another, boi!\n");
          continue;
        }
        printf("You have choosen Sting\n");
        strcpy(selection, "sting");
        break;
      case 2:
        if (thisMachine.nutriQuantity == 0) {
          printf("Out of stock, go buy another, boi!\n");
          continue;
        }
        printf("You have choosen Nutri\n");
        strcpy(selection, "nutri");
        break;
      case 3:
        if (thisMachine.monsterQuantity == 0) {
          printf("Out of stock, go buy another, boi!\n");
          continue;
        }
        printf("You have choosen Monster\n");
        strcpy(selection, "monster");
        break;
      case 0:
        beingUsed = 0;
        printf("Good bye!\n");
        exit(0);
        continue;
      default:
        printf("Invalid choice!\n");
        continue;
    }

    pid_t pid, wpid;
    int status;
    switch (pid = fork()) {
      case -1:
        perror("Error when trying to generate child process!\n");
        exit(1);
      case 0:
        // Child process
        commoditySales(selection, sock);
      default:
        // Parent process
        if (recv(sock, message, 100, 0) == 0) {
          // error: server terminated prematurely
          perror("The server terminated prematurely");
          exit(4);
        }
        handleMessage(message);
    }
  }
}

void commoditySales(char *selection, int sock) {
  send(sock, selection, strlen(selection), 0);
}

int showMenu() {
  int choice;

  printf("---------------------- \n");
  printf("Name list of Drink: ");
  printf("\n\n");
  if (thisMachine.stingQuantity == 0) {
    printf("1. Sting dau (Sold out)\n");
  } else {
    printf("1.  Sting dau (%d)\n", thisMachine.stingQuantity);
  }
  if (thisMachine.nutriQuantity == 0) {
    printf("2. Nutri (Sold out)\n");
  } else {
    printf("2.  Nutri (%d)\n", thisMachine.nutriQuantity);
  }
  if (thisMachine.monsterQuantity == 0) {
    printf("3. Monster (Sold out)\n");
  } else {
    printf("3.  Monster (%d)\n", thisMachine.monsterQuantity);
  }
  printf("0. Quit\n");
  printf("\n\n");

  printf("Enter your choice: ");
  scanf(" %d", &choice);

  return choice;
}

void handleMessage(char *message) {
  char *token = strtok(message, "|");
  int count = 1;
  switch (atoi(token)) {
    case QUANTITY_MESSAGE_CODE:

      token = strtok(NULL, "|");
      while (token != NULL) {
        switch (count) {
          case 1:
            thisMachine.stingQuantity = atoi(token);
            break;
          case 2:
            thisMachine.nutriQuantity = atoi(token);
            break;
          case 3:
            thisMachine.monsterQuantity = atoi(token);
            break;
        }
        count++;
        token = strtok(NULL, "|");
      }
      break;
    case DELIVERY:
      token = strtok(NULL, "|");
      while (token != NULL) {
        switch (count) {
          case 1:
            thisMachine.stingQuantity = atoi(token);
            break;
          case 2:
            thisMachine.nutriQuantity = atoi(token);
            break;
          case 3:
            thisMachine.monsterQuantity = atoi(token);
            break;
        }
        count++;
        token = strtok(NULL, "|");
      }
      break;
    case CONNECT_FAIL:
      perror("No more slot for you!\n");
      exit(1);
      break;
    default:
      perror("There is an error\n");
      exit(1);
      break;
  }
}
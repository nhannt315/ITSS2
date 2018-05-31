#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define MAXCLIENT 3
#define SHMSZ 27
#define MEMORY_KEY 5656

typedef struct {
  char *ip;
  int clientSock;
  int stingQuantity;
  int nutriQuantity;
  int monsterQuantity;
  int isConnected;
} VendingMachine;

typedef struct {
  VendingMachine vdList[MAXCLIENT];
} Inventory;

void initInventoryMemory();
Inventory *getInventory();
int saveInventory(Inventory *inventory);
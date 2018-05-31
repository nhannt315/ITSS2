#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define MAXCLIENT 3
#define SHMSZ 272
#define MEMORY_KEY 56567

#define CHOICE_KEY 32213
#define SHMSZ_CHOICE 231

typedef struct {
  int stingQuantity;
  int isStingDelivery;
  int nutriQuantity;
  int isNutriDelivery;
  int monsterQuantity;
  int isMonsterDelivery;
  int waitingMessage;
} VendingMachine;

void initChoiceMemory();
void *getShm();
void initInventoryMemory();
VendingMachine *getVendingMachine();
int saveVendingMachine(VendingMachine *inventory);

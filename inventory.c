#include "inventory.h"

void initInventoryMemory() {
  int shmid;
  key_t key = MEMORY_KEY;
  printf("Init memory for inventory!\n");
  /*
   * Create the segment.
   */
  if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
    perror("shmget inventory");
    exit(1);
  }
}

/*Inventory's String form Structure :
 * IP|clientSock|connected|string|nutri|monster;...*/

Inventory *getInventory() {
  int shmid;
  key_t key;
  char *shm;
  key = MEMORY_KEY;
  Inventory *inventory = malloc(sizeof(Inventory));
  /*
   * Locate the segment.
   */
  if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
    perror("shmget get inventory");
    exit(1);
  }
  /*
   * Now we attach the segment to our data space.
   */
  if ((shm = shmat(shmid, NULL, 0)) == (char *)-1) {
    perror("shmat");
    exit(1);
  }
  char *token, *rest;
  rest = malloc(300);
  strcpy(rest, shm);
  while ((token = strtok_r(rest, ";", &rest))) {
    char *token2, *rest2 = malloc(100);
    strcpy(rest2, token);
    int count = 0, countVd = 0;
    while ((token2 = strtok_r(rest2, "|", &rest2))) {
      switch (count) {
        case 0:
          if (strcmp(token2, "(null)") != 0) {
            inventory->vdList[countVd].ip = malloc(20);
            strcpy(inventory->vdList[countVd].ip, token2);
          }
          break;
        case 1:
          inventory->vdList[countVd].clientSock = atoi(token2);
          break;
        case 2:
          inventory->vdList[countVd].isConnected = atoi(token2);
          break;
        case 3:
          inventory->vdList[countVd].stingQuantity = atoi(token2);
          break;
        case 4:
          inventory->vdList[countVd].nutriQuantity = atoi(token2);
          break;
        case 5:
          inventory->vdList[countVd].monsterQuantity = atoi(token2);
        default:
          break;
      }
      count++;
    }
    countVd++;
  }
    
  return inventory;
}

int saveInventory(Inventory *inventory) {
  int shmid;
  key_t key;
  char *shm;
  key = MEMORY_KEY;
  /*
   * Locate the segment.
   */
  if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
    perror("shmget get inventory");
    exit(1);
  }
  /*
   * Now we attach the segment to our data space.
   */
  if ((shm = shmat(shmid, NULL, 0)) == (char *)-1) {
    perror("shmat");
    exit(1);
  }
  memset(shm, 0, sizeof(shm));
  printf("Write to memory\n");

  // Convert inventory to string
  int i;
  char temp[100];
  char *inventoryString = malloc(300);
  memset(inventoryString, 0, 300);
  for (i = 0; i < MAXCLIENT; i++) {
    memset(temp, 0, 100);
    sprintf(temp, "%s|%d|%d|%d|%d|%d;", inventory->vdList[i].ip,
            inventory->vdList[i].clientSock, inventory->vdList[i].isConnected,
            inventory->vdList[i].stingQuantity,
            inventory->vdList[i].nutriQuantity,
            inventory->vdList[i].monsterQuantity);
            printf("Temp : %s\n", temp);
            printf("Sting : %d\n", inventory->vdList[i].stingQuantity);
    strcat(inventoryString, temp);
  }
  printf("Save : %s\n", inventoryString);
  free(inventory);
  strcpy(shm, inventoryString);
}
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
  Inventory *ptr;
  Inventory *head;
  Inventory *result = (Inventory *)malloc(sizeof(Inventory));
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
  head = ptr = (Inventory *)shmat(shmid, 0, 0);
  if (head == (Inventory *)-1) {
    perror("shmat");
    return NULL;
  }
  memcpy(result, head, sizeof(Inventory));
  return result;
}

int saveInventory(Inventory *inventory) {
  int shmid;
  key_t key;
  Inventory *ptr;
  Inventory *head;
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
  head = ptr = (Inventory *)shmat(shmid, 0, 0);
  if (head == (Inventory *)-1) {
    perror("shmat");
    exit(1);
  }
  memcpy(head, inventory, sizeof(Inventory));
  free(inventory);
}
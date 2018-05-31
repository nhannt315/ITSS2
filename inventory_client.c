#include "inventory_client.h"

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

VendingMachine *getVendingMachine() {
  int shmid;
  key_t key;
  VendingMachine *ptr;
  VendingMachine *head;
  VendingMachine *result = (VendingMachine *)malloc(sizeof(VendingMachine));
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
  head = ptr = (VendingMachine *)shmat(shmid, 0, 0);
  if (head == (VendingMachine *)-1) {
    perror("shmat");
    return NULL;
  }
  memcpy(result, head, sizeof(VendingMachine));
  return result;
}

int saveVendingMachine(VendingMachine *vendingMachine) {
  int shmid;
  key_t key;
  VendingMachine *ptr;
  VendingMachine *head;
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
  head = ptr = (VendingMachine *)shmat(shmid, 0, 0);
  if (head == (VendingMachine *)-1) {
    perror("shmat");
    exit(1);
  }
  memcpy(head, vendingMachine, sizeof(VendingMachine));
  free(vendingMachine);
}
#include "init.h"

void *writer(int *buffer) {
  // Writer should change buffer to buffer+1 (e.g., before *buffer==0, after
  // *buffer==1)
  // Writer cannot start to work when there is any reader or writer working
  // You should implement this using semaphore
  sem_wait(&db);
  sem_wait(&rc);

  // You should output this just after this thread gets semaphore
  printf("writer gets sem\n");
  *buffer += 1;

  sem_post(&rc);
  sem_post(&db);

  // you should output this just before this thread releases the semaphore
  printf("writer writes %d and releases\n", *buffer);
}

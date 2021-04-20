#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/sem.h>

int main()
{
  int     *array;
  int     shmid;
  int     new = 1;
  char    pathname[] = "07-3a.c";
  key_t   key1;
  long    i;
  
  int    semid;
  key_t  key2;        
  struct sembuf mybuf; // Structure for specifying operations on a semaphore.

  if ((key1 = ftok(pathname,0)) < 0) {
    printf("Can\'t generate shared memory key\n");
    exit(-1);
  }

  if ((shmid = shmget(key1, 3*sizeof(int), 0666|IPC_CREAT|IPC_EXCL)) < 0) {
    if (errno != EEXIST) {
      printf("Can\'t create shared memory\n");
      exit(-1);
    } else {
      if ((shmid = shmget(key1, 3*sizeof(int), 0)) < 0) {
        printf("Can\'t find shared memory\n");
        exit(-1);
      }
      new = 0;
    }
  }

  if ((array = (int *)shmat(shmid, NULL, 0)) == (int *)(-1)) {
    printf("Can't attach shared memory\n");
    exit(-1);
  }
  
  if ((key2 = ftok(pathname,0)) < 0) {
    printf("Can\'t generate semaphores key\n");
    exit(-1);
  }
  
  
  //
  // Try to access by key the array of semaphores, if it exists,
  // or create it from a single semaphore if it does not exist yet,
  // with read & write access for all users.
  //
  if ((semid = semget(key2, 1, 0666 | IPC_CREAT | IPC_EXCL )) < 0) {
	if (errno != EEXIST){
		
      printf("Can\'t create semaphore set\n");
      exit(-1);
	  
	} else{
	  
	  semid = semget(key, 1, 0666);
	}
  } else {
	  
	  mybuf.sem_num = 0;
	  mybuf.sem_op  = 1;
	  mybuf.sem_flg = 0;
	  
	  if (semop(semid, &mybuf, 1) < 0) {
        printf("Can\'t initialize semaphore\n");
        exit(-1);
      }
	  
  }
  
  mybuf.sem_num = 0;
  mybuf.sem_op  = -1;
  mybuf.sem_flg = 0;
  
  if (semop(semid, &mybuf, 1) < 0) {
    printf("Can\'t wait for condition\n");
    exit(-1);
  }
  
  printf("Enter critical section\n");

  if (new) {
    array[0] =  1;
    array[1] =  0;
    array[2] =  1;
  } else {
    array[0] += 1;
    for(i=0; i<2000000000L; i++);
    array[2] += 1;
  }
  
  printf
    ("Program 1 was spawn %d times, program 2 - %d times, total - %d times\n",
    array[0], array[1], array[2]);
  
  mybuf.sem_num = 0;
  mybuf.sem_op  = 1;
  mybuf.sem_flg = 0;

  if (semop(semid, &mybuf, 1) < 0) {
    printf("Can\'t add 1 to semaphore\n");
    exit(-1);
  }

    printf("Exit critical section\n");

  if (shmdt(array) < 0) {
    printf("Can't detach shared memory\n");
    exit(-1);
  }

  return 0;
}

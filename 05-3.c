#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/* ___ALGORITHM___

	S = 1

	parent:
		for (0, n):
			write
			D(S, 1)
			D(S, 1)
			read
			
	child:
		for (0, n):
			Z(S)
			read
			write
			A(S, 2)

*/

int main()
{
	int    semid;        // IPC descriptor for an array of IPC semaphores
	char   pathname[]="05-3.c"; // The file name used to generate the key.
                               // A file with this name must exist in the current directory
	key_t  key;          // IPC key
	struct sembuf mybuf; // Structure for specifying operations on a semaphore.
	
    int fd[2], result;
    size_t size;
    char  resstring[14];
	
    if (pipe(fd) < 0) {
		printf("Can\'t open pipe\n");
		exit(-1);
	}
  
	if((key = ftok(pathname, 0)) < 0){
		printf("Cannot generate key\n");
		exit(-1);
	}
	
	if((semid = semget(key, 1, 0666|IPC_CREAT|IPC_EXCL)) < 0){
	  if(errno != EEXIST){
		printf("Cannot create semaphore\n");
		exit(-1);
	  } else if((semid = semget(key, 1, 0)) < 0){
		  printf("Cannot find semaphore\n");
		  exit(-1);
	  }
	} else {
		// A(S, 1)
		mybuf.sem_num = 0;
		mybuf.sem_op = 1;
		mybuf.sem_flg = 0;
		if(semop(semid, &mybuf, 1) < 0){
			printf("Cannot init semaphore\n");
			exit(-1);
		}
	}
	
	unsigned int N;
	printf("Enter N: \n");
	scanf("%u", &N);
    
    result = fork();
    
    if (result < 0) {
        printf("Can\'t fork child\n");
        exit(-1);
    } else if (result > 0) {
        
        /* Parent process */
		
		for(int i = 0; i < N; i++){
			
			//пишем в родителя
			size = write(fd[1], "Hello, world!", 14);
			
			if (size != 14) {
				printf("Can\'t write all string to pipe\n");
				exit(-1);
			}
			
			// D(S, 1)
			mybuf.sem_num = 0;
			mybuf.sem_op  = -1;
			mybuf.sem_flg = 0;
			if (semop(semid, &mybuf, 1) < 0) {
				printf("Can\'t substract 1 from semaphore\n");
				exit(-1);
			}
			
			// D(S, 1)
			mybuf.sem_num = 0;
			mybuf.sem_op  = -1;
			mybuf.sem_flg = 0;
			if (semop(semid, &mybuf, 1) < 0) {
				printf("Can\'t substract 1 from semaphore\n");
				exit(-1);
			}
			
			size = read(fd[0], resstring, 14);
			if (size != 14) {
				printf("Can\'t write all string to pipe\n");
				exit(-1);
			}
			
			printf("\nParent read from child: %s\nParent is done working!\n", resstring);
		}
    } else {
		
		for(int i = 0; i < N; i++){
			
			/* Child process */
			
			// Z(S)
			mybuf.sem_num = 0;
			mybuf.sem_op  = 0;
			mybuf.sem_flg = 0;
			if (semop(semid, &mybuf, 1) < 0) {
				printf("Can\'t wait for 0 on semaphore\n");
				exit(-1);
			}
			
			//читаем родителя
			size = read(fd[0], resstring, 14);
			
			if (size < 0) {
				printf("Can\'t read string from parent!\n");
				exit(-1);
			}
			//выводим содержимое, считанное ребенком из родителя
			printf("Child read from parent: %s\n", resstring);
			
			//пишем в ребенка ответ
			size = write(fd[1], "Bye, World....", 14);
			
			printf("Child is done working!");
			
			// A(S, 2)
			mybuf.sem_num = 0;
			mybuf.sem_op  = 2;
			mybuf.sem_flg = 0;
			if (semop(semid, &mybuf, 1) < 0) {
				printf("Can\'t wait for 0 on semaphore\n");
				exit(-1);
			}
		}
    }
    
    return 0;
}

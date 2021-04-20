#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/* ___ALGORITHM___

	S = 1

	parent:
		while (n > 0):
			write
			D(S, 1)
			D(S, 1)
			read
			
	child:
		while (n > 0):
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
  
    //создаем родителя и ребенка
    int parent[2], child[2], result;
    
    size_t size;
    char  resstring[14];
    
    //открываем pipe родителя и ребенка
    if (pipe(parent) < 0) {
        printf("Can\'t open parent pipe\n");
        exit(-1);
    }
    
    if(pipe(child) < 0){
        printf("Can\'t open child pipe\n");
    }
	
	if ((key = ftok(pathname,0)) < 0) {
		printf("Can\'t generate key\n");
		exit(-1);
    }
	
	if ((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0) {
		printf("Can\'t create semaphore set\n");
		exit(-1);
	}
	
	// A(S, 1)
	mybuf.sem_num = 0;
	mybuf.sem_op  = 1;
	mybuf.sem_flg = 0;
	if (semop(semid, &mybuf, 1) < 0) {
		printf("Can\'t initialize semaphore\n");
		exit(-1);
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
		
		while(N > 0){
		
			//закрываем на чтение родителя
			close(parent[0]);
			//закрываем на запись ребенка
			close(child[1]);
			
			
			//пишем в родителя
			size = write(parent[1], "Hello, world!", 14);
			
			if (size != 14) {
				printf("Can\'t write all string to pipe\n");
				exit(-1);
			}
			//закрываем запись родителя
			close(parent[1]);
			
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
			
			size = read(child[0], resstring, 11);
			if (size != 11) {
				printf("Can\'t write all string to pipe\n");
				exit(-1);
			}
			close(child[0]);
			
			N = N - 1;
			
			printf("\nParent read from child: %s\nParent is done working!\n", resstring);
		}
    } else {
		
		while (N > 0){
			
			/* Child process */
			
			// Z(S)
			mybuf.sem_num = 0;
			mybuf.sem_op  = 0;
			mybuf.sem_flg = 0;
			if (semop(semid, &mybuf, 1) < 0) {
				printf("Can\'t wait for 0 on semaphore\n");
				exit(-1);
			}
        
			//закрываем запись родителя
			close(parent[1]);
			//закрываем чтение ребенка
			close(child[0]);
			
			//читаем родителя
			size = read(parent[0], resstring, 14);
			
			if (size < 0) {
				printf("Can\'t read string from parent!\n");
				exit(-1);
			}
			//выводим содержимое, считанное ребенком из родителя
			printf("Child read from parent: %s\n", resstring);
			
			//пишем в ребенка ответ
			size = write(child[1], "Bye, World!", 11);
			//закрываем на запись ребенка и на чтение родителя
			close(child[1]);
			close(parent[0]);
			
			N = N - 1;
			
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

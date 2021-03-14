#include <sys/shm.h> // Shared memory
#include <sys/types.h> 
#include <stdio.h>
#include <stdlib.h> 

int main() {
    int *array;
    int shmid1, shmid2;
    char pathname1[] = "07-a.c", pathname2[] = "07-b.c";
    key_t key1, key2;
    char *code;
    
    int size = 0;
    int *file_ptr1, *file_ptr2;
    
    if ((key1  = ftok(pathname1, 0))  < 0 ||
        (key2 = ftok(pathname2, 0)) < 0) {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if ((shmid1 = shmget(key1, sizeof(int), 0)) < 0) {
        printf("Can\'t create shared memory for text size\n");
        exit(-1);
    }
    if ((file_ptr1 = (int*)shmat(shmid1, NULL, 0)) == (int*)(-1)) {
        printf("Can't attach shared memory for text size\n");
        exit(-1);
    }
    size = *file_ptr1;
    
    if ((shmid2 = shmget(key2, size * sizeof(char), 0)) < 0) {
        printf("Can\'t create shared memory for text\n");
        exit(-1);
    }
    if ((file_ptr2 = (int*)shmat(shmid2, NULL, 0)) == (int*)(-1)) {
        printf("Can't attach shared memory for text\n");
        exit(-1);
    }
	
	// Выводим считанный текст на экран
    code = (char*)file_ptr2;
    puts(code);
    
    // Открепляем общую память
    if (shmdt(file_ptr1)  < 0 ||
        shmdt(file_ptr2) < 0) {
        printf("Can't detach shared memory\n");
        exit(-1);
    }
    
    // Удаляем сегменты общей памяти
    if (shmctl(shmid1,  IPC_RMID, NULL) < 0 ||
        shmctl(shmid2, IPC_RMID, NULL) < 0) {
        printf("Can't delete shared memory\n");
        exit(-1);
    }
    return 0;
}

// Программа работает, т.к. shmget создает сегмент разделяемой памяти, размер которого равен size, округленному до числа вида k * PAGE_SIZE,
// где k - натуральное

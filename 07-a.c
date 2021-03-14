#include <sys/shm.h> // Shared memory
#include <sys/types.h> 
#include <stdio.h>
#include <stdlib.h> 

int main() {
    int shmid1, shmid2;
    char pathname1[] = "07-a.c", pathname2[] = "07-b.c";
    key_t key1, key2;
    char *source_code;
    
    int size = 0;
    int *file_ptr1, *file_ptr2;
    FILE *file;
    file = fopen(pathname1, "r");
    
    // Устанавливаем внутренний указатель на конец файла
    fseek(file, 0, SEEK_END);
    // Получаем размер файла (в байтах)
    size = ftell(file);
    // Устанавливаем указатель на начало файла
    rewind(file);
    
    if ((key1  = ftok(pathname1, 0))  < 0 ||
        (key2 = ftok(pathname2, 0)) < 0) {
        printf("Can\'t generate key\n");
        exit(-1);
    }
    // Создаем сегменты общей памяти (для размера текста и самого текста)
    if ((shmid1  = shmget(key1,  sizeof(int), 0666|IPC_CREAT)) < 0 ||
        (shmid2 = shmget(key2, size * sizeof(char), 0666|IPC_CREAT)) < 0) {
        printf("Can\'t create shared memory\n");
        exit(-1);
    }
    // Прикрепляем сегменты памяти 
    if ((file_ptr1  = (int*)shmat(shmid1,  NULL, 0)) == (int*)(-1) ||
        (file_ptr2 = (int*)shmat(shmid2, NULL, 0)) == (int*)(-1)) {
        printf("Can't attach shared memory for size\n");
        exit(-1);
    }
	// Размер текста
    *file_ptr1 = size; 
	// Текст
    source_code = (char*)file_ptr2;
    
    for (int i = 0; i < size; ++i) {
        source_code[i] = fgetc(file);
    }
	
    fclose(file);
    
    // Открепляем сегменты памяти 
    if (shmdt(file_ptr1)  < 0 ||
        shmdt(file_ptr2) < 0) {
        printf("Can't detach shared memory\n");
        exit(-1);
    }
    return 0;
}

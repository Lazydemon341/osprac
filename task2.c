#include <sys/types.h> 
#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h> 
int main() { 
int fd[2], result; 
int fd1[2]; 
size_t size; 
char string[] = "Hello, World!"; 
char string1[] = "World, Hello!"; 
char resstring[14]; 
char resstring1[14]; 
pipe(fd); 
pipe(fd1); 
result = fork(); 
if(result<0) { 
  printf("Can\'t fork child\n"); 
} else if (result > 0) { 
  close(fd[0]); 
  write(fd[1], string, 14); 
  printf("Parent writting string...(pipe 1)\n"); 
  close(fd[1]); 
  printf("Parent exit\n"); 
} else { 
  close(fd1[0]); 
  close(fd[1]); 
  printf("Child reading string...(pipe 1)\n"); 
  read(fd[0], resstring, 14); 
  printf("%s\n", resstring); 
  close(fd[0]); 
  printf("Child writting string...(pipe 2)\n"); 
  write(fd1[1], string1, 14); 
  close(fd1[1]); 
  printf("Child exit\n"); 
} 
 
sleep(3); 
if (result > 0) { 
  close(fd1[1]); 
  printf("Parent reading string...(pipe 2)\n"); 
  read(fd1[0], resstring1, 14); 
  printf("%s\n", resstring1); 
  close(fd1[0]); 
  printf("Parent exit\n"); 
} 
 
return 0; 
}

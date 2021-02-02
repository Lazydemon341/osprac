#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
  pid_t pid, ppid;
  int a = 0;

  pid = fork();

  if (pid == -1) {
    a = a+1;
    /* ошибка */
} else if (pid == 0) {
    a = a+2;
    /* ребенок */
} else {
    a = a+3;
    /* родитель */
}

  a = a+4;

  pid  = getpid();
  ppid = getppid();



  printf("My pid = %d, my ppid = %d, result = %d\n", (int)pid, (int)ppid, a);

  return 0;
}

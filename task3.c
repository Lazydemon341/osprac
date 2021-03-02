#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>

int result = 0;

int main() {
    int fd[2];
    size_t size = 1;
    char* resstring = malloc(1 * sizeof(char));

    if(pipe(fd) < 0) {
        printf("Failed to create pipe\n");
        exit(-1);
    }
    fcntl(fd[1], F_SETFL, fcntl(fd[1], F_GETFL) | O_NONBLOCK);

    size = write(fd[1], resstring, 1);
    while (size == 1) {
        size = write(fd[1], resstring, 1);
        result++;
    }
    printf("Size of pipe: %d bytes\n", result);
    return 0;
}


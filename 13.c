#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

int main(void){
    int cnt = 0;
    char pathname[] = "a";
	char prevPath[1024];
    char curPath[1024];

    int fd = open(pathname,O_RDWR|O_CREAT,0666);
    if (fd < 0){
	printf("Can\'t create file a\n");
    }
    if(close(fd) != 0){
	printf("Can\'t close file a\n");
    }
    sprintf(curPath, "a%d", cnt);
    symlink(pathname, curPath);

    while((fd = open(curPath,O_RDONLY, 0666))>= 0){
	cnt++;
	strncpy(prevPath, curPath, sizeof(prevPath));
        sprintf(curPath, "a%d", cnt);
	symlink(prevPath,curPath);
	if(close(fd)< 0){
	   printf("close error\n");
	   exit(-1);
        }
    }
    printf("Depth: %d\n", cnt);
    return 0;
}
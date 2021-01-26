#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv){
	printf("UID = %d\nGID = %d\n", getuid(), getgid());

	return 0;
}

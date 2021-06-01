#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>
#include <stdlib.h>

pid_t spid = -1; // sender pid
int numberber = 0;
int i = 0;

void check(){
	kill(spid, SIGUSR1);
	if(i == 32){
		printf("\nNumberber received: %d\n", number);
		exit(0);
	}
}

void handler_1(int nsig){
	if(spid == -1) return;
	printf("1");
	number |= 1 << i;
	i++;
	check();
}

void handler_0(int nsig){
	if(spid == -1) return;
	printf("0");
	i++;
	check();
}


int main(void){
	pid_t rpid; // receiver pid
	
	(void) signal(SIGUSR1, handler_1);
	(void) signal(SIGUSR2, handler_0);
	
	rpid = getpid();
	printf("Pid: %d\n", (int)rpid);
	
	printf("Enter sender pid: ");
	scanf("%d", &spid);
	
	while(1);
	
	return 0;
}
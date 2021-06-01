#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>
#include <stdlib.h>

pid_t rpid = -1; // receiver pid
int number = 0;
int i = 0;

void send_bit(){
	if(rpid == -1) return;
	
	int bit = 1 << i;
	if(number & bit){
		printf("Send bit 1 #%d\n", i);
		kill(rpid, SIGUSR1);
	} else {
		printf("Send bit 0 #%d\n", i);
		kill(rpid, SIGUSR2);
	}
	i++;
}

void accept(int nsig){
	if(i == 32){
		printf("Number sent\n");
		exit(0);
	} else {
		printf("Bit #%d received\n", i - 1);
		send_bit();
	}
}


int main(void){
	pid_t spid; // sender pid
	
	spid = getpid();
	printf("Pid: %d\n", (int)spid);
	
	printf("Enter receiver pid: ");
	scanf("%d", &rpid);
	
	printf("Enter number to send: ");
	scanf("%d", &number);
	
	(void) signal(SIGUSR1, accept);
	
	send_bit();
	
	while(1);
	
	return 0;
}
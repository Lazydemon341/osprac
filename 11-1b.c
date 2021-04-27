#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define LAST_MESSAGE 255

int main(void)
{
  int msqid;
  char pathname[]="11-1a.c";
  key_t  key;
  int len, maxlen;

  struct mymsgbuf
  {
    long mtype;
    struct {
		char  mtext[81];
		int   msg_id;
	} mmsg; 
  } mybuf;

  if ((key = ftok(pathname,0)) < 0) {
    printf("Can\'t generate key\n");
    exit(-1);
  }

  if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
    printf("Can\'t get msqid\n");
    exit(-1);
  }

  while (1) {
    //
    // In an infinite loop, accept messages of any type in FIFO order
    // with a maximum length of 81 characters
    // until a message of type LAST_MESSAGE is received.
    //

    if (( len = msgrcv(msqid, (struct msgbuf *) &mybuf, sizeof(mybuf.mmsg), 0, 0)) < 0) {
      printf("Can\'t receive message from queue\n");
      exit(-1);
    }
    //
    // If the received message is of type LAST_MESSAGE,
    // then terminate and remove the message queue from the system.
    // Otherwise, print the text of the received message.
    //
    if (mybuf.mtype == LAST_MESSAGE) {
      msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
      exit(0);
    }

    printf("message type = %ld, text = %s, id = %s\n", mybuf.mtype, mybuf.mmsg.mtext, mybuf.mmsg.msg_id);
  }

  return 0;
}

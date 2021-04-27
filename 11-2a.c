#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define LAST_MESSAGE 255 // Message type for termination of program 11-1b.c

int main(void)
{
  int msqid;      // IPC descriptor for the message queue
  char pathname[]="11-1a.c"; // The file name used to generate the key.
                             // A file with this name must exist in the current directory.
  key_t  key;     // IPC key
  int i, len;      // Cycle counter and the length of the informative part of the message

  struct mymsgbuf // Custom structure for the message
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
  //
  // Trying to get access by key to the message queue, if it exists,
  // or create it, with read & write access for all users.
  //
  if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
    printf("Can\'t get msqid\n");
    exit(-1);
  }

  /* Send information */

  for (i = 1; i <= 5; i++) {
    //
    // Fill in the structure for the message and
    // determine the length of the informative part.
    //
    mybuf.mtype = 1;
    strcpy(mybuf.mmsg.mtext, "This is text message");
	mybuf.mmsg.msg_id = i;
    //
    // Send the message. If there is an error,
    // report it and delete the message queue from the system.
    //
    if (msgsnd(msqid, (struct msgbuf *) &mybuf, sizeof(mybuf.mmsg), 0) < 0) {
      printf("Can\'t send message to queue\n");
      msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
      exit(-1);
    }
  }
  
  for (i = 1; i <= 5; i++) {
	if (( len = msgrcv(msqid, (struct msgbuf *) &mybuf, sizeof(mybuf.mmsg), 2, 0)) < 0) {
      		printf("Can\'t receive message from queue\n");
     		 exit(-1);
    	}
	

    printf("message type = %ld, text = %s, id = %d\n", mybuf.mtype, mybuf.mmsg.mtext, mybuf.mmsg.msg_id);
  }
  
  msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);

  return 0;
}

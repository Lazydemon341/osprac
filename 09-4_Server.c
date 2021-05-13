#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


int main() {
    int msqid;

    char pathname[] = "09-4_Server.c";

    key_t key;

    int len, maxlen;

    struct clientmsgbuf {
        long mtype;
        struct {
            pid_t pid;
            float msg;
        } info;
    } clientbuf;

    struct sermsgbuf {
        long mtype;
        struct {
            float msg;
        } info;
    } serverbuf;

    if ((key = ftok(pathname, 0)) < 0) {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
        printf("Can\'t get msqid\n");
        exit(-1);
    }

    while (1) {
        maxlen = sizeof(clientbuf.info);
        if (len = msgrcv(msqid, (struct clientmsgbuf *) &clientbuf, maxlen, 1, 0) < 0) {
            printf("Can\'t receive message from queue\n");
            exit(-1);
        }

        printf("Client %d, message: %.3f\n", clientbuf.info.pid, clientbuf.info.msg);

        serverbuf.mtype = clientbuf.info.pid;

        serverbuf.info.msg = clientbuf.info.msg * clientbuf.info.msg;
        len = sizeof(serverbuf.info);

        if (msgsnd(msqid, (struct sermsgbuf *) &serverbuf, len, 0) < 0) {
            printf("Can\'t send message to queue\n");
            msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
            exit(-1);
        }
        printf("Response sent\n");
    }
    return 0;
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#define BUFSIZE 256
int main(int argc, char *argv[]){
    char buf[BUFSIZE];
    int childtoparent[2];
    int parenttochild[2];
    int pid, msglen, status;
    if (argc != 3) {
        printf("bad argument.\n");
        exit(1);
    }
    if (pipe(parenttochild) == -1 || pipe(childtoparent) == -1) {
        perror("pipe failed.");
        exit(1);
    }
    if ((pid=fork())==-1) {
        perror("fork failed.");
        exit(1);
    }
    if (pid == 0) { /* Child process */
    close(parenttochild[1]);
    close(childtoparent[0]);
        msglen = strlen(argv[1]) + 1;
        if (write(childtoparent[1], argv[1], msglen) ==-1) {
            perror("pipe write.");
            exit(1);
        }
        if (read(parenttochild[0], buf, BUFSIZE) ==-1) {
            perror("pipe read.");
            exit(1);
        }
        printf("Message from parent process: %s\n",buf);
        wait(&status);
        exit(0);
    } else { /* Parent process */
        close(parenttochild[0]);
        close(childtoparent[1]);
        msglen = strlen(argv[2]) + 1;
        if (write(parenttochild[1], argv[2], msglen) ==-1) {
            perror("pipe write.");
            exit(1);
        }
        if (read(childtoparent[0], buf, BUFSIZE) ==-1) {
            perror("pipe read.");
            exit(1);
        }
        printf("Message from child process: %s\n",buf);
        wait(&status);
    }
}
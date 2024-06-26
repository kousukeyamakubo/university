#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define BUFSIZE 256
#define TIMEOUT 10

int timer_pid = -1; // グローバル変数でタイマー管理用のプロセスIDを保存

void myalarm(int sec) {
    if (timer_pid > 0) {
        kill(timer_pid, SIGTERM); // 子プロセスを終了させる
        waitpid(timer_pid, NULL, 0); // 子プロセスの終了を待つ（ゾンビプロセス防止）
    }
    int pid;
    time_t start_time, current_time;
    int elapsed_seconds = 0;
    if ((pid=fork())==-1) {
        perror("fork failed.");
        exit(1);
    }
    if(pid==0){/*子プロセス*/
        time(&start_time);
        while (elapsed_seconds < sec) {
            time(&current_time);
            elapsed_seconds = difftime(current_time, start_time);
            sleep(1);
        }
        if (kill(getppid(),SIGALRM) ==-1) {
            perror("kill failed.");
            exit(1);
        }
        exit(0);
    }else{
        timer_pid = pid;
    }
}

void timeout(){
    printf("This program is timeout.\n");
    exit(0);
}

int main(){
    char buf[BUFSIZE];
    if(signal(SIGALRM,timeout) == SIG_ERR) {
        perror("signal failed.");
        exit(1);
    }
    myalarm(TIMEOUT);
    while (fgets(buf, BUFSIZE, stdin) != NULL) {
        printf("echo: %s",buf);
        myalarm(TIMEOUT);
    }
}
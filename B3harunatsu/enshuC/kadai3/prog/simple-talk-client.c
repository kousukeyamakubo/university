#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>

#define BUFSIZE 256
#define TIMEOUT 10

int flag = 0;
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
    flag = 1;
}

int main(int argc, char **argv) {
    int sock, n;
    if(signal(SIGALRM,timeout) == SIG_ERR) {
        perror("signal failed.");
        close(sock);
        exit(1);
    }
    struct sockaddr_in svr;
    struct hostent *server;
    fd_set rfds;
    struct timeval tv;
    int reuse;
    char rbuf[1024];

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <hostname>\n", argv[0]);
        exit(1);
    }

    /* ソケットの生成 */
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("socket");
        exit(1);
    }

    /* ホストが存在するか確認 */
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host as %s\n", argv[1]);
        exit(0);
    }

    /* ソケットアドレス再利用の指定 */
    reuse = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt");
        exit(1);
    }
    /*サーバ受付用アドレスの設定*/
    bzero((char *)&svr, sizeof(svr));
    svr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&svr.sin_addr.s_addr, server->h_length);
    svr.sin_port = htons(10130);

    /* ソケットをサーバに接続 */
    if (connect(sock, (struct sockaddr *)&svr, sizeof(svr)) < 0) {
        perror("client: connect");
        close(sock);
        exit(1);
    }
    printf("connected\n");

    do {
        myalarm(TIMEOUT);
        /* 入力を監視するファイル記述子の集合を変数rfdsにセットする */
        FD_ZERO(&rfds);
        FD_SET(0, &rfds); /* 標準入力 */
        FD_SET(sock, &rfds); /* ソケット */
        /* 監視する待ち時間を10秒に設定 */
        tv.tv_sec = 15;
        tv.tv_usec = 0;
        /* 標準入力とソケットからの受信を同時に監視する */
        if (select(sock + 1, &rfds, NULL, NULL, &tv) > 0) {
            if (FD_ISSET(0, &rfds)) {
                bzero(rbuf, 1024);
                if (fgets(rbuf, 1024, stdin) == NULL) {
                    if (timer_pid > 0) {
                        kill(timer_pid, SIGTERM); // 子プロセスを終了させる
                        waitpid(timer_pid, NULL, 0); // 子プロセスの終了を待つ（ゾンビプロセス防止）
                    }
                    printf("\nEOF detected\n");
                    break;
                }
                n = write(sock, rbuf, strlen(rbuf));
                if (n < 0) {
                    perror("ERROR writing");
                    break;
                }
            }
            if (FD_ISSET(sock, &rfds)) {
                bzero(rbuf, 1024);
                n = read(sock, rbuf, 1024);
                if(n <= 0){
                    perror("Connection closed by client.\n");
                    close(sock);
                    return 0;
                }else{
                    printf("%s",rbuf);
                }
            }
        }
    } while (!flag);
    if(flag == 1){
        printf("This program is timeout.\n");
    }
    close(sock);
    return 0;
}

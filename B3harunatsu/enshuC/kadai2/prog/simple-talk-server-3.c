#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc,char **argv) {
    int sock,csock,n,nsock;
    struct sockaddr_in svr;
    struct sockaddr_in clt;
    struct hostent *cp;
    fd_set rfds;
    struct timeval tv;
    int clen,nbytes,reuse;
    char rbuf[1024];
    char sbuf[1024];
    char hostname[1024];

    if (gethostname(hostname, sizeof(hostname)) < 0) {
        perror("gethostname");
        exit(1);
    }
    /* ソケットの生成 */
    if ((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0) {
        perror("socket");
        exit(1);
    }
    /* ソケットアドレス再利用の指定 */
    reuse=1;
    if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse))<0) {
        perror("setsockopt");
        exit(1);
    }
    /* client 受付用ソケットの情報設定 */
    bzero(&svr,sizeof(svr));
    svr.sin_family=AF_INET;
    svr.sin_addr.s_addr=htonl(INADDR_ANY); /* 受付側の IP アドレスは任意 */
    svr.sin_port=htons(10130); /* ポート番号 10130 を介して受け付ける */
    /* ソケットにソケットアドレスを割り当てる */
    if(bind(sock,(struct sockaddr *)&svr,sizeof(svr))<0) {
        perror("bind");
        exit(1);
    }
    /* 待ち受けクライアント数の設定 */
    if (listen(sock,5)<0) { /* 待ち受け数に5を指定 */
        perror("listen");
        exit(1);
    }
    /* クライアントの受付 */
    clen = sizeof(clt);
    if ( ( csock = accept(sock,(struct sockaddr *)&clt,&clen) ) <0 ) {
        perror("accept");
        exit(2);
    }
    /* クライアントのホスト情報の取得 */
    cp = gethostbyaddr((char *)&clt.sin_addr,sizeof(struct in_addr),AF_INET);
    printf("connected with [%s]\n",cp->h_name);
    do{
        /* 入力を監視するファイル記述子の集合を変数rfdsにセットする */
        FD_ZERO(&rfds);
        FD_SET(0,&rfds);
        /* rfds を空集合に初期化 */
        /* 標準入力 */
        FD_SET(csock,&rfds); /* クライアントを受け付けたソケット */
        /* 監視する待ち時間を1秒に設定 */
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        /* 標準入力とソケットからの受信を同時に監視する */
        if(select(csock+1,&rfds,NULL,NULL,&tv)>0) {
            if(FD_ISSET(0,&rfds)) { /* 標準入力から入力があったなら */
            /* 標準入力から読み込みクライアントに送信 */
                bzero(rbuf, 1024);
                if(fgets(rbuf, 1024,stdin) == NULL){
                    printf("\nEOF detected\n");
                    close(csock);
                    exit(1);
                }
                snprintf(sbuf, sizeof(sbuf), "[%s]: %s", hostname, rbuf);
                n = write(csock, sbuf, strlen(sbuf));
                if(n < 0){
                    perror("ERROR writing");
                    break;
                }
            }
            if(FD_ISSET(csock,&rfds)) { /* ソケットから受信したなら */
            /* ソケットから読み込み端末に出力 */
            bzero(rbuf, 1024);
            n = read(csock, rbuf, 1024);
            if(n <= 0){
                perror("Connection closed by client.\n");
                close(csock);
                return 0;
            }else{
                printf("%s",rbuf);
            }
            }
            if (FD_ISSET(sock, &rfds)) { /* 新しい接続要求があったなら */
                nsock = accept(sock, (struct sockaddr *)&clt, &clen);
                if (nsock >= 0) {
                    cp = gethostbyaddr((char *)&clt.sin_addr, sizeof(struct in_addr), AF_INET);
                    if (cp) {
                        printf("Connection attempt from [%s] rejected\n", cp->h_name);
                    } else {
                        printf("Connection attempt from [%s] rejected\n", inet_ntoa(clt.sin_addr));
                    }
                    const char *msg = "Server is currently busy. Try again later.\n";
                    write(nsock, msg, strlen(msg));
                    close(nsock);
                }
            }
        }
    } while(1); /* 繰り返す */

    close(sock);
    return 0;    
}
#include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <netdb.h>
 #include <unistd.h>
 #include <string.h>
 #include <stdio.h>
 #include <stdlib.h>
 int main(int argc,char **argv) {
    int sock,n;
    struct sockaddr_in svr;
    struct hostent *server;
    int reuse;
    char rbuf[1024];

    if(argc != 2){
        fprintf(stderr, "Usage is not correct\n");
        exit(1);
    }

    /* ソケットの生成 */
    if ((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0) {
        perror("socket");
        exit(1);
    }

    /*ホストが存在するか確認*/
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", argv[1]);
        exit(0);
    }

    /* ソケットアドレス再利用の指定 */
    reuse=1;
    if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse))<0) {
        perror("setsockopt");
        exit(1);
    }

    /*ソケットの接続先の情報設定*/
    bzero((char *)&svr, sizeof(svr));
    svr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&svr.sin_addr.s_addr, server->h_length);
    svr.sin_port = htons(10120);
    
    /* ソケットをサーバに接続 */
    if (connect(sock, (struct sockaddr *)&svr, sizeof(svr)) < 0) {
        perror("client: connect");
        close(sock);
        exit(1);
    }

    while(1){
        printf("enter message:");
        bzero(rbuf, 1024);
        if(fgets(rbuf, 1024,stdin) == NULL){
            printf("\nEOF detected\n");
            break;
        }
        /*メッセージをサーバーに送信*/
        n = write(sock, rbuf, strlen(rbuf));
        if(n < 0){
            perror("ERROR writing");
            break;
        }

        /*メッセージをサーバーから受信*/
        bzero(rbuf, 1024);
        n = read(sock, rbuf, 1024);
        if(n < 0){
            perror("ERROR READING");
            break;
        }
        printf("%s",rbuf);
        /* ソケットをクローズ */
    } 
    close(sock);
    return 0;
 }

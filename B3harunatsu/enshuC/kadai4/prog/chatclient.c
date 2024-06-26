#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    int sock,n;
    char rbuf[1024];
    fd_set rfds;
    struct timeval tv;
    struct hostent *server;
    if(argc != 3){
        fprintf(stderr, "Usage: %s <hostname> <username>\n", argv[0]);
        exit(1);
    }

    /*状態1*/
    if ((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0) {
        perror("socket");
        exit(1);
    }
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host as %s\n", argv[1]);
        exit(0);
    }
    bzero((char *)&svr, sizeof(svr));
    svr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&svr.sin_addr.s_addr, server->h_length);
    svr.sin_port = htons(10140);
    if (connect(sock, (struct sockaddr *)&svr, sizeof(svr)) < 0) {
        perror("client: connect");
        close(sock);
        exit(1);
    }

    /*状態2*/
    bzero(rbuf, 1024);
    n = read(csock, rbuf, 1024);
    if(n <= 0){
        perror("Connection closed by client.\n");
        close(csock);
        return 0;
    }elseif(strcmp(rbuf,"REQUEST ACCEPTED\n") == 0){/*状態3*/
        n = write(sock, argv[2], strlen(argv[2]));
        if(n < 0){
            perror("ERROR writing");
            break;
        }
        n = read(csock, rbuf, 1024);
        if(n <= 0){
            perror("Connection closed by client.\n");
            close(csock);
            return 0;
        }
        if(strcmp(rbuf,"USERNAME REGISTERED\n") == 0){/*状態4*/
            FD_ZERO(&rfds);
            FD_SET(0, &rfds);
            FD_SET(sock, &rfds);
            tv.tv_sec = 1;
            tv.tv_usec = 0;
            while(1){/*入力された文字列(改行など)をそのまま送るという処理を実装していない？*/
                if (select(sock + 1, &rfds, NULL, NULL, &tv) > 0) {
                    if (FD_ISSET(0, &rfds)) {
                        bzero(rbuf, 1024);
                        if (fgets(rbuf, 1024, stdin) == NULL) {/*状態5*/
                            printf("\nEOF detected\n");
                            close(sock);
                            exit(0);
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
            }
        }else{/*状態6*/
            close(sock);
            exit(1);
        }
    }else{/*状態6*/
        close(sock);
        exit(1);
    }

    return 0;
}
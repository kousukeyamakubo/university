#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXCLIENTS 5

int main(){
    int sock,n,k,clen,max_sd,new_socket;
    int csock[5];
    struct sockaddr_in svr;
    struct sockaddr_in clt;
    fd_set rfds;
    struct timeval tv;
    char rbuf[1024];
    char accept_request_message[] = "REQUEST ACCEPTED\n";
    char reject_request_message[] = "REQUEST REJECTED\n";
    char accept_username_message[] = "USERNAME ACCEPTED\n";
    char reject_username_message[] = "USERNAME REJECTED\n";

    /*状態1*/
    if ((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0) {
        perror("socket");
        exit(1);
    }
    bzero(&svr,sizeof(svr));
    svr.sin_family=AF_INET;
    svr.sin_addr.s_addr=htonl(INADDR_ANY);
    svr.sin_port=htons(10140);
    if(bind(sock,(struct sockaddr *)&svr,sizeof(svr))<0) {
        perror("bind");
        exit(1);
    }
    if (listen(sock,5)<0) { /* 待ち受け数に5を指定 */
        perror("listen");
        exit(1);
    }
    k=0;

    /*状態2*/
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    FD_SET(sock, &rfds);
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    max_sd = sock;
    while(1){
        for(int i=0;i<MAXCLIENTS;i++){
            sd = csock[i];
            if(sd > 0){
                FD_SET(sd, &rfds);
            }
            if (sd > max_sd) {
                max_sd = sd;
            }
        }
        if (select(max_sd + 1, &rfds, NULL, NULL, &tv) > 0) {
            /*状態3*/
            if (FD_ISSET(sock, &rfds)) {/*状態4*/
                clen = sizeof(clt);
                if ((new_socket = accept(sock, (struct sockaddr *)&clt, &clen)) < 0) {
                    perror("accept");
                    exit(2);
                }
                if(k<MAXCLIENTS){
                    n = write(sock, accept_request_message, strlen(accept_request_message));
                    if(n < 0){
                        perror("ERROR writing");
                        break;
                    }
                    for (int i = 0; i < MAXCLIENTS; i++) {
                        if (csock[i] == -1) {
                            csock[i] = new_socket;
                            break;
                        }
                    }

                    /*状態5*/
                    bzero(rbuf, 1024);
                    n = read(csock, rbuf, 1024);//登録するユーザー名を取得
                    if(n <= 0){
                        perror("Connection closed by client.\n");
                        close(csock);
                        return 0;
                    }else{
                        for(int i=0;i<MAXCLIENTS;i++){//登録するユーザー名が既に使用されていないかを確認
                            if(){
                                n = write(sock, accept_username_message, strlen(accept_username_message));
                                if(n < 0){
                                    perror("ERROR writing");
                                    break;
                                }
                                k++;
                            }else{
                                n = write(sock, reject_username_message, strlen(reject_username_message));
                                if(n < 0){
                                    perror("ERROR writing");
                                    break;
                                }
                                close(sock);
                            }
                        }
                    }
                }else{
                    n = write(sock, reject_request_message, strlen(reject_request_message));
                    if(n < 0){
                        perror("ERROR writing");
                        break;
                    }
                    close(sock);
                } 
            }
            for(int i=0;i<MAXCLIENTS;i++){
                if (FD_ISSET(csock[i], &rfds)){/*状態5*/

                }
            }
        }
            
    }


    

    return 0;
}
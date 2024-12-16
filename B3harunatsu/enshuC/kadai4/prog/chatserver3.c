#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define MAXCLIENTS 5

int main(){
    int sock,n,k,sd,clen,max_sd,new_socket,new_socket_num,username_flag;
    int csock[5];
    char registered_username[MAXCLIENTS][256];
    //IPアドレス用の配列
    char client_ip[MAXCLIENTS][INET_ADDRSTRLEN];
    //現在の時刻を取得するための変数
    time_t rawtime;
    struct tm *timeinfo;
    char time_buffer[80];

    struct sockaddr_in svr;
    struct sockaddr_in clt;
    fd_set rfds;
    struct timeval tv;
    char rbuf[1024];
    char name_message[1024];
    char accept_request_message[] = "REQUEST ACCEPTED\n";
    char reject_request_message[] = "REQUEST REJECTED\n";
    char accept_username_message[] = "USERNAME REGISTERED\n";
    char reject_username_message[] = "USERNAME REJECTED\n";
    
    /*状態1*/
    for (int i = 0; i < MAXCLIENTS; i++) {
        csock[i] = -1;
        bzero(registered_username[i],256);
    }
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
    while(1){
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        FD_SET(sock, &rfds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        max_sd = sock;
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
                    n = write(new_socket, accept_request_message, strlen(accept_request_message));
                    if(n < 0){
                        perror("ERROR writing");
                        close(new_socket);
                        break;
                    }
                    
                    /*状態5*/
                    for (int i = 0; i < MAXCLIENTS; i++) {//登録可能なソケット番号を確認
                        if (csock[i] == -1) {
                            new_socket_num = i;
                            csock[i] = new_socket;
                            break;
                        }
                    }
                    
                    bzero(rbuf, 1024);
                    n = read(new_socket, rbuf, 1024);//登録するユーザー名を取得
                    if(n <= 0){
                        perror("Connection closed by client.\n");
                        close(new_socket);
                        return 0;
                    }
                    username_flag = 0;
                    for(int i=0;i<1024;i++){
                        if(rbuf[i] == '\n'){
                            rbuf[i] = '\0';
                            break;
                        }
                        if(!isalnum(rbuf[i]) && rbuf[i] != '_' && rbuf[i] != '_'){
                            username_flag = 1;
                        }
                    }
                    for(int i=0;i<MAXCLIENTS;i++){//登録するユーザー名が既に使用されていないかを確認
                        if (strcmp(registered_username[i], rbuf) == 0) {
                            username_flag = 1;
                            break;
                        }
                    }
                    if(username_flag == 0){
                        n = write(new_socket, accept_username_message, strlen(accept_username_message));
                        if(n < 0){
                            perror("ERROR writing");
                            close(new_socket);
                            break;
                        }
                        inet_ntop(AF_INET,&clt.sin_addr, client_ip[new_socket_num], INET_ADDRSTRLEN);
                        strcpy(registered_username[new_socket_num], rbuf);
                        printf("%s is registered\n",registered_username[new_socket_num]);
                        csock[new_socket_num] = new_socket;
                        k++;
                        //登録されたことを送信
                        bzero(rbuf,1024);
                        strcat(rbuf,registered_username[new_socket_num]);
                        strcat(rbuf," is registered\n");
                        strncat(rbuf, "---------------------------\n",sizeof(rbuf) - strlen(rbuf) -1);
                        strncat(rbuf, "Current participating Users\n",sizeof(rbuf) - strlen(rbuf) -1);
                        for(int j=0;j<MAXCLIENTS;j++){
                            if(csock[j] != -1){
                                strcat(rbuf,"・");
                                strcat(rbuf,registered_username[j]);
                                strcat(rbuf,"\n");
                            }
                        }
                        strncat(rbuf, "---------------------------\n",sizeof(rbuf) - strlen(rbuf) -1);
                        for(int j=0;j<MAXCLIENTS;j++){
                            if(csock[j] != -1){
                                n = write(csock[j], rbuf, strlen(rbuf));
                                if(n < 0){
                                    perror("ERROR writing");
                                    break;
                                }
                            }
                        }

                    }else{
                        n = write(new_socket, reject_username_message, strlen(reject_username_message));
                        if(n < 0){
                            perror("ERROR writing");
                            close(new_socket);
                            break;
                        }
                        csock[new_socket_num] = -1;
                        close(new_socket);
                    }
                }else{
                    n = write(new_socket, reject_request_message, strlen(reject_request_message));
                    if(n < 0){
                        perror("ERROR writing");
                        close(new_socket);
                        break;
                    }
                    close(new_socket);
                } 
            }
            for(int i=0;i<MAXCLIENTS;i++){
                if ((csock[i] != -1) && (FD_ISSET(csock[i], &rfds))){/*状態6*/
                    //ソケットから受信したなら
                    bzero(rbuf, 1024);
                    //時刻を取得
                    time(&rawtime);
                    timeinfo = localtime(&rawtime);
                    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

                    n = read(csock[i], rbuf, 1024);
                    if(n <= 0){/*状態7*/
                        close(csock[i]);
                        printf("%s has left the chat room\n",registered_username[i]);
                        k--;
                        bzero(rbuf,1024);
                        snprintf(rbuf, sizeof(rbuf), "%s has left the chatroom\n",registered_username[i]);
                        csock[i] = -1;
                        bzero(client_ip[i],INET_ADDRSTRLEN);
                        bzero(registered_username[i],256);
                        strncat(rbuf, "---------------------------\n",sizeof(rbuf) - strlen(rbuf) -1);
                        strncat(rbuf, "Current participating Users\n",sizeof(rbuf) - strlen(rbuf) -1);
                        for(int j=0;j<MAXCLIENTS;j++){
                            if(csock[j] != -1){
                                strcat(rbuf,"・");
                                strcat(rbuf,registered_username[j]);
                                strcat(rbuf,"\n");
                            }
                        }
                        strncat(rbuf, "---------------------------\n",sizeof(rbuf) - strlen(rbuf) -1);
                        for(int j=0;j<MAXCLIENTS;j++){
                            if(csock[j] != -1){
                                n = write(csock[j], rbuf, strlen(rbuf));
                                if(n < 0){
                                    perror("ERROR writing");
                                    break;
                                }
                            }
                        }
                        break;
                    }else{
                        for(int j=0;j<MAXCLIENTS;j++){
                            if(csock[j] != -1){
                                snprintf(name_message, sizeof(name_message), ">[%s %s (%s)] %s", time_buffer, registered_username[i],client_ip[i], rbuf);
                                n = write(csock[j], name_message, strlen(name_message));
                                if(n < 0){
                                    perror("ERROR writing");
                                    break;
                                }
                            }
                        }  
                    }
                }
            }
        }            
    }
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/wait.h>
#define NUMPROCS 4
char filename[]="counter";

void lock(int semid){
    struct sembuf op;
    /*セマフォの設定*/
    op.sem_num = 0;
    op.sem_op =-1;
    op.sem_flg = 0;
    if (semop(semid, &op, 1) == -1) {
        perror("semop lock");
        exit(1);
    }
}

void unlock(int semid){
    struct sembuf op;
    op.sem_num = 0; // セマフォの番号を固定値として指定
    op.sem_op = 1; // V操作（加算、アンロック）
    op.sem_flg = 0;
    if (semop(semid, &op, 1) == -1) {
        perror("semop V");
        exit(1);
    }
}

int count1(){
    FILE *ct;
    int count;
    if ((ct=fopen(filename, "r"))==NULL) exit(1);
    fscanf(ct, "%d\n", &count);
    count++;
    fclose(ct);
    if ((ct=fopen(filename, "w"))==NULL) exit(1);
    fprintf(ct, "%d\n", count);
    fclose(ct);
    return count;
}

int main(){
    int i, count, pid, status, sem_id;
    FILE *ct;
    key_t key;
    struct sembuf sb;
    setbuf(stdout, NULL); /* set stdout to be unbufferd */
    if ((key = ftok(".", 1)) ==-1){
        fprintf(stderr,"ftok path does not exist.\n");
        exit(1);
    }
    //セマフォセットを作成
    if ((sem_id=semget(key, 4, 0666 | IPC_CREAT)) ==-1) {
        perror("semget error.");
        exit(1);
    }
    // セマフォの初期値を1に設定
    if (semctl(sem_id, 0, SETVAL, 1) == -1) {
        perror("semctl SETVAL error.");
        exit(1);
    }
    count = 0;

    if ((ct=fopen(filename, "w"))==NULL) exit(1);
    fprintf(ct, "%d\n", count);
    fclose(ct);
    for (i=0; i<NUMPROCS; i++) {
        if ((pid=fork())==-1) {
            perror("fork failed.");
            exit(1);
        }
        if (pid == 0) { /* Child process */
            lock(sem_id);
            count = count1();
            printf("count = %d\n", count);
            unlock(sem_id);
            exit(0);
        }
    }
    for (i=0; i<NUMPROCS; i++) {
    wait(&status);
    }

    // セマフォを削除
    if (semctl(sem_id, 0, IPC_RMID) == -1) {
        perror("semctl IPC_RMID error.");
        exit(1);
    }
    exit(0);
}
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUMPROCS 4 /* プロセスの数 */

int main() {
    int i, sem_id_barrier, sem_id_counter, pid, status;

    // セマフォ（バリア到達待ち）を作成
    sem_id_barrier = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    if (sem_id_barrier == -1) {
        perror("semget (barrier) error.");
        exit(1);
    }

    // セマフォ（カウンター）を作成
    sem_id_counter = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    if (sem_id_counter == -1) {
        perror("semget (counter) error.");
        exit(1);
    }

    // セマフォの初期値を0に設定
    if (semctl(sem_id_barrier, 0, SETVAL, 0) == -1) {
        perror("semctl (barrier) SETVAL error.");
        exit(1);
    }

    // セマフォの初期値を1に設定
    if (semctl(sem_id_counter, 0, SETVAL, 1) == -1) {
        perror("semctl (counter) SETVAL error.");
        exit(1);
    }

    for (i = 0; i < NUMPROCS; i++) {
        pid = fork();
        if (pid == -1) {
            perror("fork failed.");
            exit(1);
        } else if (pid == 0) { // 子プロセス
            // カウンターセマフォを待ち状態にする
            struct sembuf sb_counter_wait;
            sb_counter_wait.sem_num = 0;
            sb_counter_wait.sem_op = -1;
            sb_counter_wait.sem_flg = 0;
            if (semop(sem_id_counter, &sb_counter_wait, 1) == -1) {
                perror("semop (counter) error.");
                exit(1);
            }

            // バリアに到達
            printf("Process %d arrived at the barrier\n", getpid());

            // カウンターセマフォの値を減らす
            struct sembuf sb_counter_dec;
            sb_counter_dec.sem_num = 0;
            sb_counter_dec.sem_op = -1;
            sb_counter_dec.sem_flg = 0;
            if (semop(sem_id_counter, &sb_counter_dec, 1) == -1) {
                perror("semop (counter) error.");
                exit(1);
            }

            // カウンターセマフォの値が0ならば、バリアセマフォを解放する
            if (semctl(sem_id_counter, 0, GETVAL) == 0) {
                struct sembuf sb_barrier_post;
                sb_barrier_post.sem_num = 0;
                sb_barrier_post.sem_op = 1;
                sb_barrier_post.sem_flg = 0;
                if (semop(sem_id_barrier, &sb_barrier_post, 1) == -1) {
                    perror("semop (barrier) error.");
                    exit(1);
                }
            }

            // カウンターセマフォを解放
            struct sembuf sb_counter_inc;
            sb_counter_inc.sem_num = 0;
            sb_counter_inc.sem_op = 1;
            sb_counter_inc.sem_flg = 0;
            if (semop(sem_id_counter, &sb_counter_inc, 1) == -1) {
                perror("semop (counter) error.");
                exit(1);
            }

            // バリア到達待ち
            struct sembuf sb_barrier_wait;
            sb_barrier_wait.sem_num = 0;
            sb_barrier_wait.sem_op = -1;
            sb_barrier_wait.sem_flg = 0;
            if (semop(sem_id_barrier, &sb_barrier_wait, 1) == -1) {
                perror("semop (barrier) error.");
                exit(1);
            }

            // バリアを通過
            printf("Process %d passed the barrier\n", getpid());
            exit(0);
        }
    }

    // 親プロセスがすべての子プロセスの終了を待機する
    for (i = 0; i < NUMPROCS; i++) {
        wait(&status);
    }

    // セマフォを削除
    if (semctl(sem_id_barrier, 0, IPC_RMID) == -1) {
        perror("semctl (barrier) IPC_RMID error.");
        exit(1);
    }
    if (semctl(sem_id_counter, 0, IPC_RMID) == -1) {
        perror("semctl (counter) IPC_RMID error.");
        exit(1);
    }

    return 0;
}

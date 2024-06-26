#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

#define NUM 5

int A_list[] = {1, 2, 3, 4, 5};
int B_list[] = {6, 7, 8, 9, 10};

void lock(int semid) {
    struct sembuf op;
    op.sem_num = 0;  // セマフォの番号
    op.sem_op = -1;  // P操作（減算、ロック）
    op.sem_flg = 0;
    if (semop(semid, &op, 1) == -1) {
        perror("semop lock");
        exit(1);
    }
}

void unlock(int semid) {
    struct sembuf op;
    op.sem_num = 0;  // セマフォの番号
    op.sem_op = 1;   // V操作（加算、アンロック）
    op.sem_flg = 0;
    if (semop(semid, &op, 1) == -1) {
        perror("semop unlock");
        exit(1);
    }
}

int main() {
    int i, a_pid, b_pid, sem_id;
    key_t key;

    // セマフォ用のキーを生成
    if ((key = ftok(".", 1)) == -1) {
        fprintf(stderr, "ftok パスが存在しません。\n");
        exit(1);
    }

    // セマフォセットを1つ作成
    if ((sem_id = semget(key, 1, 0666 | IPC_CREAT)) == -1) {
        perror("semget エラー.");
        exit(1);
    }

    // セマフォを初期値1で初期化
    if (semctl(sem_id, 0, SETVAL, 1) == -1) {
        perror("semctl SETVAL エラー.");
        exit(1);
    }

    // 最初の子プロセスをフォーク
    if ((a_pid = fork()) == -1) {
        perror("フォークに失敗しました.");
        exit(1);
    }

    if (a_pid == 0) { // 子プロセスA
        for (int i = 0; i < NUM; i++) {
            lock(sem_id);
            printf("a%d\n", A_list[i]);
            unlock(sem_id);
        }
        exit(0);
    }

    // 2番目の子プロセスをフォーク
    if ((b_pid = fork()) == -1) {
        perror("フォークに失敗しました.");
        exit(1);
    }

    if (b_pid == 0) { // 子プロセスB
        for (int i = 0; i < NUM; i++) {
            lock(sem_id);
            printf("b%d\n", B_list[i]);
            unlock(sem_id);
        }
        exit(0);
    }

    // 親プロセスは両方の子プロセスの終了を待つ
    wait(NULL);
    wait(NULL);

    // セマフォを解放
    if (semctl(sem_id, 0, IPC_RMID) == -1) {
        perror("semctl IPC_RMID エラー.");
        exit(1);
    }

    return 0;
}

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define N 1000        // 往返次数，足够大以降低 tick 精度误差
#define TICKS_PER_SEC 10   // time 10MHz，每 1e6 一次中断 => 10 ticks/秒

int main(int argc, char *argv[])
{
    int p2c[2];   // parent -> child
    int c2p[2];   // child -> parent
    char buf[128];

    if (pipe(p2c) < 0 || pipe(c2p) < 0) {
        fprintf(2, "pingpong: pipe failed\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "pingpong: fork failed\n");
        exit(1);
    }

    if (pid == 0) {
        /* 子进程：只做管道收发，不打印 */
        close(p2c[1]);
        close(c2p[0]);

        for (int i = 0; i < N; i++) {
            if (read(p2c[0], buf, 1) != 1) {
                exit(1);
            }
            if (write(c2p[1], buf, 1) != 1) {
                exit(1);
            }
        }

        close(p2c[0]);
        close(c2p[1]);
        exit(0);
    } else {
        /* 父进程：只做管道收发，不打印 */
        close(p2c[0]);
        close(c2p[1]);

        buf[0] = 'x';

        int start = uptime();
        for (int i = 0; i < N; i++) {
            if (write(p2c[1], buf, 1) != 1) {
                exit(1);
            }
            if (read(c2p[0], buf, 1) != 1) {
                exit(1);
            }
        }
        int end = uptime();

        int ticks = end - start;
        if (ticks > 0) {
            int per_sec = (N * TICKS_PER_SEC) / ticks;
            // int per_sec = ticks/(TICKS_PER_SEC*N);
            printf("pingpong: %d round-trips in %d ticks, ~%d exchanges/sec\n",
                   N, ticks, per_sec);
        } else {
            printf("pingpong: elapsed ticks too small, increase N\n");
        }

        close(p2c[1]);
        close(c2p[0]);
        wait(0);
        exit(0);
    }
}
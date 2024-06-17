// df -h | tail -n +2 | cut -d ' ' -f 1 | sort -u

#include <err.h>
#include <unistd.h>

void safePipe(int pipefds[2]);
pid_t safeFork(void);

void safePipe(int pipefds[2]) {
        if (pipe(pipefds) < 0) {
                err(1, "can't pipe");
        }
}

pid_t safeFork(void) {
        pid_t pid = fork();
        if (pid < 0) {
                err(2, "can't fork");
        }
        return pid;
}

int main(void) {
        pid_t pid;

        int dfToTail[2];
        safePipe(dfToTail);

        pid = safeFork();

        if (pid == 0) {
                // df -h
                close(dfToTail[0]);

                dup2(dfToTail[1], 1);
                close(dfToTail[1]);

                execlp("df", "df", "-h", (char*)NULL);
                err(3, "can't df");
        }

        close(dfToTail[1]);

        int tailToCut[2];
        safePipe(tailToCut);

        pid = safeFork();

        if (pid == 0) {
                // tail -n +2
                dup2(dfToTail[0], 0);
                close(dfToTail[0]);

                close(tailToCut[0]);

                dup2(tailToCut[1], 1);
                close(tailToCut[1]);

                execlp("tail", "tail", "-n", "+2", (char*)NULL);
                err(4, "can't tail");
        }

        close(dfToTail[0]);
        close(tailToCut[1]);

        int cutToSort[2];
        safePipe(cutToSort);

        pid = safeFork();

        if (pid == 0) {
                // cut -d ' ' -f 1
                dup2(tailToCut[0], 0);
                close(tailToCut[0]);

                close(cutToSort[0]);

                dup2(cutToSort[1], 1);
                close(cutToSort[1]);

                execlp("cut", "cut", "-d", " ", "-f", "1", (char*)NULL);
                err(5, "can't cut");
        }

        close(tailToCut[0]);
        close(cutToSort[1]);

        dup2(cutToSort[0], 0);
        close(cutToSort[0]);

        execlp("sort", "sort", "-u", (char*)NULL);
        err(6, "can't sort");
}

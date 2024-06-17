// df -h | cut -d ' ' -f 1

#include <err.h>
#include <unistd.h>
#include <sys/wait.h>

void safePipe(int pipefd[2]);
pid_t safeFork(void);
void safeWait(void);

void safePipe(int pipefd[2]) {
        if (pipe(pipefd) < 0) {
                err(1, "can't pipe");
        }
}

pid_t safeFork() {
        pid_t pid = fork();
        if (pid < 0) {
                err(2, "can't fork");
        }
        return pid;
}

void safeWait() {
        int status;
        wait(&status);

        if (WIFEXITED(status)) {
                if (WEXITSTATUS(status) != 0) {
                        errx(8, "df was not successful- code: %d", WEXITSTATUS(status));
                }
        } else {
                errx(7, "df was killed");
        }

}

int main(void) {
        int dfToCut[2];

        safePipe(dfToCut);

        pid_t pid1 = safeFork();

        if (pid1 == 0) {
                //df -h
                close(dfToCut[0]);
                dup2(dfToCut[1], 1);
                close(dfToCut[1]);

                execlp("df", "df", "-h", (char*)NULL);
                err(3, "can't exec");
        }

        //cut -d " " -f 1
        close(dfToCut[1]);
        dup2(dfToCut[0], 0);
        close(dfToCut[0]);

        safeWait(); // not needed

        execlp("cut", "cut", "-d", " ", "-f", "1", (char*)NULL);
        err(4, "can't cut");
}

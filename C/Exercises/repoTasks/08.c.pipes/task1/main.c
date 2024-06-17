#include <err.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char** argv) {
        if (argc < 1 + 1) {
                errx(1, "at least 1 arg expected");
        }

        int pipefd[2];

        if (pipe(pipefd) < 0) {
                err(2, "failed to create pipe");
        }

        pid_t pid;

        if ((pid = fork()) < 0) {
                err(3, "failed to fork");
        }

        if (pid == 0) {
                close(pipefd[1]);

                int res;
                char c;

                while ((res = read(pipefd[0], &c, sizeof(c))) > 0) {
                        if (c == '\0') {
                                break;
                        }

                        if (write(1, &c, sizeof(c)) < 0) {
                                err(5, "failed while writing");
                        }
                }

                c = '\n';

                if (write(1, &c, sizeof(c)) < 0) {
                        err(5, "failed while writing");
                }

                if (res < 0) {
                        err(4, "failed while reading");
                }

                close(pipefd[0]);

                exit(0);
        }

        close(pipefd[0]);

        if (write(pipefd[1], argv[1], strlen(argv[1]) + 1) < 0) {
                err(5, "failed while writing");
        }

        int status;
        wait(&status);

        if (WIFEXITED(status)) {
                if (WEXITSTATUS(status) != 0) {
                        errx(8, "child did not exit normally- code %d", WEXITSTATUS(status));
                }
        } else {
                errx(7, "cild was killed");
        }

        close(pipefd[1]);

        return 0;
}

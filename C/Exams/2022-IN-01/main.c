#include <err.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#include <stdio.h>

int main(int argc, char** argv) {
        if (argc != 2 + 1) {
                errx(1, "args should be 2");
        }

        int N = atoi(argv[1]);

        if (N == 0) {
                return 0;
        }

        int D = atoi(argv[2]);

        int parentToChild[2];
        if (pipe(parentToChild) < 0) {
                err(2, "can't pipe");
        }

        int childToParent[2];
        if (pipe(childToParent) < 0) {
                err(2, "can't pipe");
        }

        pid_t pid = fork();

        if (pid < 0) {
                err(3, "can't fork");
        }

        if (pid == 0) {
                // child
                close(parentToChild[1]);
                close(childToParent[0]);

                char isOn;      // '1' or '0'

                while (1) {
                        if (read(parentToChild[0], &isOn, sizeof(isOn)) < 0) {
                                err(4, "can't read");
                        }

                        if (isOn == '0') {
                                break;
                        }

                        dprintf(1, "DONG\n");

                        if (write(childToParent[1], &isOn, sizeof(isOn)) < 0) {
                                err(5, "can't write");
                        }
                }

                close(parentToChild[0]);
                close(childToParent[1]);
                exit(0);
        }

        // parent
        close(parentToChild[0]);
        close(childToParent[1]);

        char isOn = '1';        // '1' or '0'

        for (int i = 0; i < N; i++) {
                dprintf(1, "DING\n");

                if (write(parentToChild[1], &isOn, sizeof(isOn)) < 0) {
                        err(5, "can't write");
                }

                if (read(childToParent[0], &isOn, sizeof(isOn)) < 0) {
                        err(4, "can't read");
                }

                sleep(D);
        }

        isOn = '0';

        if (write(parentToChild[1], &isOn, sizeof(isOn)) < 0) {
                err(5, "can't write");
        }

        int status;
        if (wait(&status) < 0) {
                err(6, "can't wait");
        }

        if (WIFEXITED(status)) {
                if (WEXITSTATUS(status) != 0) {
                        errx(8, "child did not exit successfully- code %d", WEXITSTATUS(status));
                }
        } else {
                errx(7, "child was killed");
        }

        close(parentToChild[1]);
        close(childToParent[0]);

        return 0;
}

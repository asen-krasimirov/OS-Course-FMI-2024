#include <err.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

#include <stdio.h>

int main(int argc, char** argv) {
        if (argc != 1 + 1) {
                errx(1, "1 arg expected");
        }

        int findToParent[2];

        if (pipe(findToParent) < 0) {
                err(2, "can't pipe");
        }

        pid_t pid = fork();

        if (pid < 0) {
                err(3, "can't fork");
        }

        if (pid == 0) {
                // find . -type f -not -name "*.hash"
                close(findToParent[0]);
                dup2(findToParent[1], 1);
                close(findToParent[1]);

                execlp("find", "find", argv[1], "-type", "f", "-not", "-name", "*.hash", (char*)NULL);
                err(4, "can't execlp find");
        }

        close(findToParent[1]);

        int res;
        char buffer[1024];
        size_t idx = 0;
        char c;
        size_t childrenCtr = 0;

        while ((res = read(findToParent[0], &c, sizeof(c))) > 0) {
                buffer[idx++] = c;

                if (c == '\n') {
                        buffer[idx - 1] = '\0';
                        idx = 0;

                        childrenCtr++;

                        pid = fork();

                        if (pid < 0) {
                                err(3, "can't fork");
                        }

                        if (pid == 0) {
                                // child
                                int md5ToChild[2];

                                if (pipe(md5ToChild) < 0) {
                                        err(2, "can't pipe");
                                }

                                pid = fork();

                                if (pid < 0) {
                                        err(3, "can't fork");
                                }

                                if (pid == 0) {
                                        // md5sum
                                        close(md5ToChild[0]);
                                        dup2(md5ToChild[1], 1);
                                        close(md5ToChild[1]);

                                        execlp("md5sum", "md5sum", buffer, (char*)NULL);
                                        err(4, "can't execlp md5sum");
                                }

                                close(md5ToChild[1]);

                                char hashOfFile[32];

                                if (read(md5ToChild[0], hashOfFile, 32) < 0) {
                                        err(6, "can't read");
                                }

                                char* hashFileName = strcat(buffer, ".hash");

                                int hashFileDesc = open(hashFileName, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);

                                if (hashFileDesc < 0) {
                                        err(5, "can't open");
                                }

                                if (write(hashFileDesc, hashOfFile, 32) < 0) {
                                        err(7, "can't read");
                                }

                                close(md5ToChild[0]);
                                close(hashFileDesc);
                                wait(NULL);
                                exit(0);
                        }
                }

        }

        if (res < 0) {
                err(6, "can't read");
        }

        close(findToParent[0]);

        // wait children

        int status;

        for (size_t i = 0; i < childrenCtr; ++i) {
                wait(&status);

                if (WIFEXITED(status)) {
                        if (WEXITSTATUS(status) != 0) {
                                errx(9, "child did not exit normally- code: %d", WEXITSTATUS(status));
                        }
                } else {
                        errx(8, "child was killed");
                }
        }

        return 0;
}

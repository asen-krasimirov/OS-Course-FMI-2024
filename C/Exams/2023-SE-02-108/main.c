#include <unistd.h>
#include <err.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

const char* STRING_TO_FIND = "found it!";

int main(int argc, char** argv) {
        pid_t childPids[1024];
        pid_t pid;

        int childToParent[2];

        if (pipe(childToParent) < 0) {
                err(26, "can't pipe");
        }

        for (int i = 1; i <= argc; ++i) {
                pid = fork();

                if (pid < 0) {
                        err(26, "can't fork");
                }

                childPids[i] = pid;

                if (pid == 0) {
                        // child

                        close(childToParent[0]);
                        dup2(childToParent[1], 1);
                        close(childToParent[1]);

                        execlp(argv[i], argv[i], (char*)NULL);
                        err(26, "can't execlp %s", argv[i]);
                }

        }
        // parent

        close(childToParent[1]);

        int res;
        char buffer[4096];
        size_t idx = 0;
        char c;
        bool isFound = false;

        while ((res = read(childToParent[0], &c, sizeof(c))) > 0) {
                buffer[idx++] = c;

                if (c == '\0') {
                        if (strcmp(buffer, STRING_TO_FIND) == 0) {
                                isFound = true;
                                break;
                        }
                        idx = 0;
                }
        }

        if (res < 0) {
                err(26, "can't read");
        }

        if (isFound) {
                for (int i = 1; i <= argc; ++i) {
                        if (kill(childPids[i], 15) < 0) {
                                err(26, "can't kill child #%d", childPids[i]);
                        }
                }
        }

        for (int i = 1; i <= argc; ++i) {
                wait(NULL);
        }

        close(childToParent[0]);

        return 0;
}

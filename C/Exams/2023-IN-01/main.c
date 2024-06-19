#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>

const char* L[3] = {
        "tic ",
        "tac ",
        "toe\n"
};

void printTic(int ctr);

void printTic(int ctr) {
        if (write(1, L[ctr % 3], 4) < 0) {
                err(4, "can't write");
        }
}

int main(int argc, char** argv) {
        if (argc != 2 + 1) {
                errx(1, "2 args expected");
        }

        char* endptr;

        int NC = strtol(argv[1], &endptr, 10);

        if (NC < 1 || NC > 7) {
                errx(2, "NC should be between 1-7");
        }

        int WC = strtol(argv[2], &endptr, 10);

        if (WC < 1 || WC > 35) {
                errx(3, "WC should be between 1-35");
        }

        int parentToChildFds[8][2];     // we skip the first, saving it for parent
        int childToParentFds[8][2];

        pid_t pid;
        int ctr = 1;

        while (ctr < NC + 1) {
                if (pipe(parentToChildFds[ctr]) < 0) {
                        err(6, "can't pipe");
                }

                if (pipe(childToParentFds[ctr]) < 0) {
                        err(6, "can't pipe");
                }

                pid = fork();

                if (pid < 0) {
                        err(5, "can't fork");
                }

                if (pid == 0) {
                        // child
                        for (int i = 1; i < ctr; ++i) {
                                if (parentToChildFds[i][1] != parentToChildFds[ctr][1]) {
                                        close(parentToChildFds[i][1]);
                                }

                                if (childToParentFds[i][0] != childToParentFds[ctr][0]) {
                                        close(childToParentFds[i][0]);
                                }
                        }

                        close(parentToChildFds[ctr][1]);
                        close(childToParentFds[ctr][0]);

                        int wrdCtr = 0;

                        while (1) {
                                if (read(parentToChildFds[ctr][0], &wrdCtr, sizeof(wrdCtr)) < 0) {
                                        err(7, "can't read");
                                }

                                if (wrdCtr < 0) {
                                        break;
                                }

                                printTic(wrdCtr);

                                if (write(childToParentFds[ctr][1], &wrdCtr, sizeof(wrdCtr)) < 0) {
                                        err(4, "can't write");
                                }


                        }

                        close(parentToChildFds[ctr][0]);
                        close(childToParentFds[ctr][1]);
                        exit(0);
                }


                close(parentToChildFds[ctr][0]);
                close(childToParentFds[ctr][1]);
                ctr++;
        }
        // parent

        int toPrintIdx;
        ctr = 0;

        while (ctr < WC) {
                toPrintIdx = ctr % (NC + 1);

                if (toPrintIdx == 0) {
                        printTic(ctr);
                } else {
                        if (write(parentToChildFds[toPrintIdx][1], &ctr, sizeof(ctr)) < 0) {
                                err(7, "can't read");
                        }

                        if (read(childToParentFds[toPrintIdx][0], &ctr, sizeof(ctr)) < 0) {
                                err(7, "can't read");
                        }

                }

                ctr++;
        }

        ctr = -1;

        for (int i = 1; i < NC + 1; ++i) {
                if (write(parentToChildFds[i][1], &ctr, sizeof(ctr)) < 0) {
                        err(4, "can't write");
                }
        }

        for (int i = 1; i < NC + 1; ++i) {
                close(parentToChildFds[i][1]);
                close(childToParentFds[i][0]);
        }

        return 0;
}



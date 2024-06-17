#include <stdint.h>
#include <err.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#include <stdio.h>

int openFile(char* name, int flags, int mode);
size_t getFileSize(int fd);
off_t safeLseek(int fd, off_t offset, int whence);

int openFile(char* name, int flags, int mode) {
        int fd = open(name, flags, mode);
        if (fd < 0) {
                err(3, "can't open");
        }
        return fd;
}

size_t getFileSize(int fd) {
        struct stat s;
        if (fstat(fd, &s) < 0) {
                err(2, "can't stat");
        }
        return s.st_size;
}

off_t safeLseek(int fd, off_t offset, int whence) {
        off_t res;
        if ((res = lseek(fd, offset, whence)) < 0) {
                err(7, "can't offset");
        }
        return res;
}

typedef struct {
        char name[8];
        uint32_t off;
        uint32_t len;
} tuple;

int main(int argc, char** argv) {
        if (argc != 1 + 1) {
                errx(1, "1 arg should be given");
        }

        int inputDesc = openFile(argv[1], O_RDONLY, 0);

        size_t inputSize = getFileSize(inputDesc);

        if (inputSize % sizeof(tuple) != 0) {
                errx(4, "wrong input data format");
        }

        if (inputSize > sizeof(tuple) * 8) {
                errx(4, "wrong input data fomrat");
        }

        int res;
        tuple curTuple;
        uint16_t readXor = 0;
        uint16_t totalXor = 0;

        int child_count = 0;

        pid_t pid;
        int pipefd[2];

        if (pipe(pipefd) < 0) {
                err(5, "can't pipe");
        }

        while ((res = read(inputDesc, &curTuple, sizeof(curTuple))) > 0) {
                //printf("xor file: %s , offset: %d , len: %d\n", curTuple.name, curTuple.off, curTuple.len);
                child_count++;

                if ((pid = fork()) < 0) {
                        err(6, "can't fork");
                }

                if (pid == 0) {
                        // child - xor file
                        close(inputDesc);
                        close(pipefd[0]);

                        uint16_t curXor = 0;
                        uint16_t curNum = 0;
                        size_t readCtr = 0;

                        int curFileDesc = openFile(curTuple.name, O_RDONLY, 0);
                        size_t curFileSize = getFileSize(curFileDesc);

                        if (curTuple.off + curTuple.len > curFileSize) {
                                errx(4, "wrong input data format");
                        }

                        safeLseek(curFileDesc, sizeof(uint16_t) * curTuple.off, SEEK_SET);

                        while ((readCtr < curTuple.len) && ((res = read(curFileDesc, &curNum, sizeof(curNum)))) > 0) {
                                curXor ^= curNum;
                                readCtr++;
                        }

                        if (res < 0) {
                                err(8, "can't read");
                        }

                        if (write(pipefd[1], &curXor, sizeof(curXor)) < 0) {
                                err(9, "can't write");
                        }

                        close(curFileDesc);
                        close(pipefd[1]);
                        exit(0);
                }
        }

        close(pipefd[1]);

        while ((res = read(pipefd[0], &readXor, sizeof(readXor))) > 0) {
                totalXor ^= readXor;
        }

        if (res < 0) {
                err(8, "can't read");
        }

        close(pipefd[0]);

        int status;

        for (int i = 0; i < child_count; i++) {
                if((pid = wait(&status)) == -1) {
                        err(1, "wait");
                }

                if (WIFEXITED(status)) {
                        if(WEXITSTATUS(status) != 0) {
                                warnx("child %d exited with code %d", pid, WEXITSTATUS(status));
                        }
                } else {
                        warnx("child %d was killed", pid);
                }
        }

        dprintf(1, "result: %.4x\n", totalXor);

        close(inputDesc);

        return 0;
}

#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <stdio.h>

int safeOpen(char*, int, int);
void safeLseek(int, int, int);

int safeOpen(char* fileName, int flags, int mode) {
        int fd;
        if ((fd = open(fileName, flags, mode)) < 0) {
                err(2, "failed while opening a file");
        }
        return fd;
}

void safeLseek(int fd, int offset, int whence) {
        if (lseek(fd, offset, whence) < 0 ){
                err(6, "error occured while lseeking");
        }
}

int main(int argc, char** argv) {
        if (argc != 3 + 1) {
                errx(1, "args should be 3");
        }

        char* wordToFind = argv[1];
        char* dictFileName = argv[2];
        char* idxFileName = argv[3];

        int fd1 = safeOpen(dictFileName, O_RDONLY, 0);
        int fd2 = safeOpen(idxFileName, O_RDONLY, 0);

        uint32_t indexes[1024];
        uint32_t idx;
        size_t wordCount = 0;
        int res;

        while ((res = read(fd2, &idx, sizeof(idx))) > 0) {
                indexes[wordCount++] = idx;
        }

        if (res < 0) {
                err(3, "error occured while reading from file");
        }

        int l = 0;
        int r = wordCount;
        int m;

        char curWord[64];
        char c;

        bool isWordFound = false;

        while (l <= r) {
                m = l + (r - l) / 2;

                safeLseek(fd1, indexes[m] + 1, SEEK_SET);

                idx = 0;

                while ((res = read(fd1, &c, sizeof(c))) > 0) {
                        if (c == '\n') {
                                break;
                        }

                        if (idx == 64) {
                                errx(5, "word too big");
                        }

                        curWord[idx++] = c;
                }

                curWord[idx] = '\0';

                if (res == -1) {
                        err(3, "error occured while reading from file");
                }

                int cmpRes = strcmp(wordToFind, curWord);

                if (cmpRes == 0) {
                        isWordFound = true;

                        while ((res = read(fd1, &c, sizeof(c))) > 0) {
                                res = write(1, &c, sizeof(c));

                                if (res < 0) {
                                        err(4, "error occured while writing to stdout");
                                }

                                if (c == '\0') {
                                        dprintf(1, "\n");
                                        break;
                                }
                        }

                        if (res < 0) {
                                err(3, "error occured while reading from file");
                        }

                        break;
                } else if (cmpRes > 0) {
                        l = m + 1;
                } else {
                        r = m - 1;
                }
        }

        if (!isWordFound) {
                dprintf(1, "Word not found in dict!\n");
        }

        close(fd1);
        close(fd2);

        return 0;
}

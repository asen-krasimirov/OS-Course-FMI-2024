#include <err.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int safeOpen(const char*, int, int);
void safePrintNumsFromFile(int, const char*);
void safePrintBiggestNumFromFile(int, const char*);
void safePrintSmallestNumFromFile(int, const char*);

int safeOpen(const char* fileName, int mode, int rights) {
        int fd = open(fileName, mode, rights);

        if (fd < 0) {
                err(2, "could not open file, %s", fileName);
        }

        return fd;
}

void safePrintSmallestNumFromFile(int fd, const char* fileName) {
        uint16_t smallest = UINT16_MAX;
        uint16_t curNum;
        int res;

        while ((res = read(fd, &curNum, sizeof(uint16_t))) > 0) {
                if (curNum < smallest) {
                        smallest = curNum;
                }
        }

        if (res < 0) {
                err(5, "could not read from file, %s", fileName);
        }

        dprintf(1, "%d\n", smallest);
}

void safePrintBiggestNumFromFile(int fd, const char* fileName) {
        uint16_t biggest = 0;
        uint16_t curNum;
        int res;

        while ((res = read(fd, &curNum, sizeof(uint16_t))) > 0) {
                if (curNum > biggest) {
                        biggest = curNum;
                }
        }

        if (res < 0) {
                err(5, "could not read from file, %s", fileName);
        }

        dprintf(1, "%d\n", biggest);
}

void safePrintNumsFromFile(int fd, const char* fileName) {
        uint16_t num;
        int res;

        while ((res = read(fd, &num, sizeof(uint16_t))) > 0) {
                dprintf(1, "%d\n", num);
        }

        if (res < 0) {
                err(4, "could not read from file, %s", fileName);
        }
}

int main(int argc, char* argv[]) {
        if (argc != 2 + 1) {
                errx(1, "should take 2 args");
        }

        char* option = argv[1];
        char* fileName = argv[2];

        int fd = safeOpen(fileName, O_RDONLY, 0);

        if (strcmp(option, "--min") == 0) {
                safePrintSmallestNumFromFile(fd, fileName);
        } else if (strcmp(option, "--max") == 0) {
                safePrintBiggestNumFromFile(fd, fileName);
        } else if (strcmp(option, "--print") == 0) {
                safePrintNumsFromFile(fd, fileName);
        } else {
                errx(1, "first arg should be one of: --min, --max, --print");
        }

        close(fd);

        return 0;
}

#include <err.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

// #include <stdio.h>

#define UINT16_SIZE 16

int safeOpen(const char* name, int flags, int mode);
off_t getFileSize(int fd);

int safeOpen(const char* name, int flags, int mode) {
        int fd = open(name, flags, mode);
        if (fd < 0) {
                err(2, "could not open file");
        }
        return fd;
}

off_t getFileSize(int fd) {
        struct stat s;
        if (fstat(fd, &s) < 0) {
                err(3, "could not stat file");
        }
        return s.st_size;
}

int main(int argc, char** argv) {
        if (argc != 2 + 1) {
                errx(1, "args should be 2");
        }

        const char* sclName = argv[1];
        const char* sdlName = argv[2];
        const char* resName = "result.out";

        int sclDesc = safeOpen(sclName, O_RDONLY, 0);
        int sdlDesc = safeOpen(sdlName, O_RDONLY, 0);
        int resDesc = safeOpen(resName, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

        int sclSize = getFileSize(sclDesc);
        int sdlSize = getFileSize(sdlDesc);

        if (sdlSize % sizeof(uint16_t) != 0) {
                errx(4, "wrong format of input data");
        }

        if (sclSize * UINT16_SIZE != sdlSize) {
                errx(4, "wrong format of input data");
        }

        int res = 0;
        int8_t byte;
        int mask;
        uint16_t curCheckNum;

        while ((res = read(sclDesc, &byte, sizeof(byte))) > 0) {
                for (mask = 128; mask > 0; mask = mask >> 1) {
                        if (read(sdlDesc, &curCheckNum, sizeof(curCheckNum)) < 0) {
                                err(5, "error while reading from file: %s", sdlName);
                        }

                        if ((byte & mask) > 0) {
                                if (write(resDesc, &curCheckNum, sizeof(curCheckNum)) < 0) {
                                        err(6, "error while writing to file: %s", resName);
                                }

                        }
                }
        }

        if (res < 0) {
                err(5, "error while reading from file: %s", sclName);
        }


        close(sclDesc);
        close(sdlDesc);
        close(resDesc);

        return 0;
}

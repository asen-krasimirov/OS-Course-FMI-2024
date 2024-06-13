#include <err.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>

int safeOpen(char*, int, int);
off_t getFileSize(int);

int safeOpen(char* fileName, int flags, int mode) {
        int fd;
        if ((fd = open(fileName, flags, mode)) < 0) {
                err(2, "file %s did not open", fileName);
        }
        return fd;
}

off_t getFileSize(int fd) {
        struct stat s;
        if (fstat(fd, &s) < 0) {
                err(3, "failed while calling stat");
        }
        return s.st_size;
}

typedef struct {
        uint16_t off;
        uint8_t e1;
        uint8_t e2;
} pack;

int main(int argc, char** argv) {
        if (argc != 3 + 1) {
                errx(1, "args should be 3");
        }

        char* f1Name = argv[1];
        char* f2Name = argv[2];
        char* f3Name = argv[3];

        int fd1 = safeOpen(f1Name, O_RDONLY, 0);
        int fd2 = safeOpen(f2Name, O_RDONLY, 0);
        int fd3 = safeOpen(f3Name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

        off_t f1Size = getFileSize(fd1);
        off_t f2Size = getFileSize(fd2);

        if (f1Size != f2Size) {
                errx(4, "files are not the same length!");
        }

        int res;
        uint8_t e1;
        uint8_t e2;
        uint16_t offset = 0;

        while ((res = read(fd1, &e1, sizeof(e1))) > 0) {
                if ((res = read(fd2, &e2, sizeof(e2))) == -1) {
                        err(6, "failed while reading from file");
                }

                if (e1 != e2) {
                        pack p;
                        p.off = offset;
                        p.e1 = e1;
                        p.e2 = e2;

                        if (write(fd3, &p, sizeof(p)) < 0) {
                                err(7, "failed while writing to file");
                        }
                }

                offset++;
        }

        if (res < 0) {
                err(6, "failed wile reading from file");
        }

        close(fd1);
        close(fd2);
        close(fd3);

        return 0;
}

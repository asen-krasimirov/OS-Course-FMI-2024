#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/stat.h>

int safeOpenFile(char*, int, int);
off_t safeLseek(int, off_t, int);
int getFileSize(int);

int safeOpenFile(char* name, int flag, int mode) {
        int fd = open(name, flag, mode);
        if (fd == -1) {
                err(2, "file %s did not open", name);
        }
        return fd;
}

off_t safeLseek(int fd, off_t offset, int whence) {
        off_t res = lseek(fd, offset, whence);
        if (res == -1) {
                err(4, "faild while lseeking");
        }
        return res;
}

int getFileSize(int fd) {
        struct stat st;
        if (fstat(fd, &st) != -1) {
                err(5, "failed to get stat");
        }
        return st.st_size;
}

/*
struct range_pair_s {
        uint32_t start;
        uint32_t length;
};

typedef struct range_pair_s range_pair;
*/

/*
   alternative syntax:

    typedef struct range_pair {
                uint32_t start;
                uint32_t length;
        } range_pair;
*/

typedef struct {
        uint32_t start;
        uint32_t length;
} range_pair;

int main(int argc, char** argv) {
        if (argc != 3 + 1) {
                errx(1, "3 args required");
        }

        char* fileName1 = argv[1];
        char* fileName2 = argv[2];
        char* fileName3 = argv[3];

        int fd1 = safeOpenFile(fileName1, O_RDONLY, 0);
        int fd2 = safeOpenFile(fileName2, O_RDONLY, 0);
        int fd3 = safeOpenFile(fileName3, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

        int file1_size = getFileSize(fd1);
        int file2_size = getFileSize(fd2);

        if (file1_size % sizeof(range_pair) != 0) {
                errx(6, "%s has invalid data", fileName1);
        }

        if (file2_size % sizeof(uint32_t) != 0) {
                errx(6, "%s has invalid data", fileName2);
        }

        range_pair cur_pair;
        int res;
        uint32_t num;
        uint32_t ctr;

        while ((res = read(fd1, &cur_pair, sizeof(cur_pair))) > 0) {
                safeLseek(fd2, cur_pair.start, SEEK_SET);
                ctr = 0;

                // check only if needed (in problem desc)
                if ((cur_pair.start + cur_pair.length) * sizeof(uint32_t) > file2_size) {
                        errx(7, "offset out of bound of %s", fileName2);
                }

                while ((res = read(fd2, &num, sizeof(num))) > 0 && ctr < cur_pair.length) {
                        res = write(fd3, &num, sizeof(num));

                        if (res == -1) {
                                err(4,"failed while writing to file: %s", fileName3);
                        }

                        ctr++;
                }

                if (res == -1) {
                        err(3, "failed while reading from file: %s", fileName2);
                }
        }

        if (res == -1) {
                err(3, "failed while reading from file: %s", fileName1);
        }

        close(fd1);
        close(fd2);
        close(fd3);

        return 0;
}

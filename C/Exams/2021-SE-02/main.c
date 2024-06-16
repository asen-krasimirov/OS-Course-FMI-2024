#include <err.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

int safeOpen(const char* name, int flags, int mode);
off_t getFileSize(int fd);

int safeOpen(const char* name, int flags, int mode) {
        int fd = open(name, flags, mode);
        if (fd < 0) {
                err(2, "error occured while opening file: %s", name);
        }
        return fd;
}

off_t getFileSize(int fd) {
        struct stat s;
        if (fstat(fd, &s) < 0) {
                err(3, "error occured while stat on file");
        }
        return s.st_size;
}

int main(int argc, char** argv) {
        if (argc != 2 + 1) {
                errx(1, "args should be 2");
        }

        const char* inputName = argv[1];
        const char* outputName = argv[2];

        int inputDesc = safeOpen(inputName, O_RDONLY, 0);
        int outputDesc = safeOpen(outputName, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

        int inputSize = getFileSize(inputDesc);

        if (inputSize % sizeof(int16_t) != 0) {
                errx(4, "input data not formated good");
        }

        int res = 0;
        uint8_t byte;
        uint8_t toSave = 0;     // 0000 0000
        int bitsEncrypted = 0;

        while ((res = read(inputDesc, &byte, sizeof(byte))) > 0) {
                for (int mask = 128; mask > 0; mask = mask >> 2) {
                        if ((byte & mask) > 0) {
                                toSave++;
                        }

                        if (++bitsEncrypted == 8) {
                                if (write(outputDesc, &toSave, sizeof(toSave)) < 0) {
                                        err(6, "failed while writing to file: %s", outputName);
                                }

                                bitsEncrypted = 0;
                                toSave = 0;
                                break;
                        }

                        toSave = toSave << 1;
                }

        }

        if (res < 0) {
                err(5, "failed while reading form file: %s", inputName);
        }

        close(inputDesc);
        close(outputDesc);

        return 0;
}

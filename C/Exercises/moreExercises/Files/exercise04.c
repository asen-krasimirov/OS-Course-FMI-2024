#include <err.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int safeOpen(const char*, int, int);

int safeOpen(const char* fileName, int mode, int rights) {
        int fd = open(fileName, mode, rights);

        if (fd < 0) {
                err(1, "chould not open file, %s", fileName);
        }

        return fd;
}

int main(void) {
        const char targetName[12] = "/etc/passwd";
        const char copyName[10] = "passwdCpy";

        int targetD = safeOpen(targetName, O_RDONLY, 1);
        int toD = safeOpen(copyName, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);

        char c;
        char oldDelim = ':';
        char newDelim = '?';
        int res;

        while ((res = read(targetD, &c, sizeof(c))) > 0) {
                if (c == oldDelim) {
                        res = write(toD, &newDelim, sizeof(newDelim));
                } else {
                        res = write(toD, &c, sizeof(c));
                }

                if (res < 0) {
                        err(1, "error occured while writing to file, %s", copyName);
                }
        }

        if (res < 0) {
                err(1, "error occured while reading from file, %s", targetName);
        }

        close(targetD);
        close(toD);
}

// Реализирайте команда cp, работеща с два аргумента, подадени като входни параметри.

/*
1. check args count
2. create 2nd arg (or trunc if already created)
3. cpy 1st arg to 2nd arg
*/

#include <err.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int safeOpen(const char*, int, int);
void safeCopyContent(int, const char*, int, const char*);

int safeOpen(const char* fileName, int mode, int rights) {
        int fn = open(fileName, mode, rights);

        if (fn < 0) {
                err(2, "file %s could not open", fileName);
        }

        return fn;
}

void safeCopyContent(int fromD, const char* fromName, int toD, const char* toName) {
        char c;
        int res;

        while ((res = read(fromD, &c, sizeof(c))) == sizeof(c)) {
                res = write(toD, &c, sizeof(c));

                if (res < 0) {
                        err(4, "failed while writing to file %s", toName);
                }
        }

        if (res < 0) {
                err(3, "failed while reading from file %s", fromName);
        }

        if (lseek(fromD, 0, SEEK_SET) < 0) {
                err(4, "failed while lseeking in file %s", fromName);
        }

        if (lseek(toD, 0, SEEK_SET) < 0) {
                err(5, "failed while lseeking in file %s", toName);
        }
}

int main(int argc, char* argv[]) {
        if (argc != 2 + 1) {
                errx(1, "args should be 2");
        }

        char *fromName = argv[1];
        char *toName = argv[2];

        int toD = safeOpen(toName, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
        int fromD = safeOpen(fromName, O_RDONLY, 0);

        safeCopyContent(fromD, fromName, toD, toName);

        close(toD);
        close(fromD);
}

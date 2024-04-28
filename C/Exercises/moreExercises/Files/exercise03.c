// Реализирайте команда cp, работеща с произволен брой подадени входни параметри.

#include <err.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <stdio.h>

int safeOpen(const char*, int, int);
void safeCopyContent(int, const char*, int, const char*);
char* parseFilePath(const char*, const char*);
void safeCopyContentIntoFolder(const char*, const char*);

int safeOpen(const char* fileName, int mode, int rights) {
        int fn = open(fileName, mode, rights);

        if (fn < 0) {
                err(2, "%s could not open", fileName);
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

char* parseFilePath(const char* fileName, const char* folderName) {
        char* filePath = malloc(strlen(fileName) + strlen(folderName) + 3);
        filePath = strcpy(filePath, folderName);
        filePath = strcat(filePath, "/\0");
        filePath = strcat(filePath, fileName);
        return filePath;
}

void safeCopyContentIntoFolder(const char* fromName, const char* folderName) {
        char* newFilePath = parseFilePath(fromName, folderName);

        int toD = safeOpen(newFilePath, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);

        int fromD = safeOpen(fromName, O_RDONLY, 0);

        safeCopyContent(fromD, fromName, toD, newFilePath);

        free(newFilePath);

        close(toD);
        close(fromD);
}

int main(int argc, char* argv[]) {
        if (argc < 2 + 1) {
                errx(1, "args should be at least 2");
        }

        char *toName = argv[argc - 1];

        if (argc == 2 + 1) {
                int toD = safeOpen(toName, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);

                int fromD = safeOpen(argv[1], O_RDONLY, 0);

                safeCopyContent(fromD, argv[1], toD, toName);

                close(toD);
                close(fromD);
        }
        else {
                for (int i = 1; i < argc - 1; i++) {
                        safeCopyContentIntoFolder(argv[i], toName);
                }
        }

        return 0;
}

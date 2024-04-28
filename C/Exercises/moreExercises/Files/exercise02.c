#include <err.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
        if (argc == 1) {
                errx(1, "should take at least 1 arg");
        }

        for (int i = 1; i < argc; i++) {
                int fd = open(argv[i], O_RDONLY);

                if (fd < 0) {
                        err(2, "file could not open %s", argv[i]);
                }

                char c;
                int res;

                while ((res = read(fd, &c, sizeof(c))) == sizeof(c)) {
                        res = write(1, &c, sizeof(c));

                        if (res < 0) {
                                err(4, "file %s failed while writing", argv[i]);
                        }
                }

                if (res < 0) {
                        err(3, "file %s failed while reading", argv[i]);
                }

                close(fd);
        }
}

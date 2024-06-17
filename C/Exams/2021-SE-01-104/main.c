#include <sys/time.h>
#include <err.h>
#include <time.h>
#include <unistd.h>
#include <pwd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include <stdio.h>

int main(int argc, char** argv) {
        struct timeval tv;

        if (gettimeofday(&tv, NULL) < 0) {
                err(1, "failed to gettimeofday");
        }

        struct tm* t = localtime(&tv.tv_sec);

        if (t == NULL) {
                err(2, "failed while localtime");
        }

        char date[26];

        strftime(date, 26, "%Y-%m-%d %H:%M:%S", t);

        uid_t curUid = getuid();
        struct passwd* userData = getpwuid(curUid);
        char* username = userData->pw_name;

        int logDesc;
        if ((logDesc = open("foo.log", O_WRONLY)) < 0) {
                err(3, "failed while opening file");
        }

        if (lseek(logDesc, 0, SEEK_END) < 0) {
                err(4, "failed while lseeking");
        }

        if (dprintf(logDesc, "%s.%ld %s", date, tv.tv_usec, username) < 0) {
                err(5, "can't write");
        }

        for (int i = 0; i < argc; ++i) {
                if (dprintf(logDesc, " %s", argv[i]) < 0) {
                        err(5, "can't write");
                }
        }

        if (dprintf(logDesc, "\n") < 0) {
                err(5, "can't write");
        }

        pid_t pid;

        if ((pid = fork()) < 0) {
                err(6, "can't fork");
        }

        if (pid == 0) {
                execlp("echo", "echo", "passwd", "-l", username, (char*)NULL);
        }

        int status;
        wait(&status);

        if (WIFEXITED(status)) {
                if (WEXITSTATUS(status) != 0) {
                        errx(8, "passwd did not exit successfully- code: %d", WEXITSTATUS(status));
                }
        } else {
                errx(7, "child was killed");
        }

        if ((pid = fork()) < 0) {
                err(6, "can't fork");
        }

        if (pid == 0) {
                execlp("echo", "echo", "killall", "-u", username, (char*)NULL);
        }

        wait(&status);

        if (WIFEXITED(status)) {
                if (WEXITSTATUS(status) != 0) {
                        errx(8, "killall did not exit successfully- coe: %d", WEXITSTATUS(status));
                }
        } else {
                errx(7, "child was killed");
        }

        close(logDesc);

        return 0;
}

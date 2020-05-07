#include <syslog.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/file.h>

#define LOCKFILE "/var/run/daemon.pid"
#define LOCKMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int lockfile(int fd) {
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    return (fcntl(fd, F_SETLK, &fl));
}

int already_running(void) {
    syslog(LOG_ERR, "Check if daemon isn't running");

    int fd;
    char buf[16];

    fd = open(LOCKFILE, O_RDWR | O_CREAT, LOCKMODE);

    if (fd < 0) {
        syslog(LOG_ERR, "couldn't open %s: %s", LOCKFILE, strerror(errno));
        exit(1);
    }

    syslog(LOG_WARNING, "Lock-файл opened");

    flock(fd, LOCK_EX | LOCK_UN);
    if (errno == EWOULDBLOCK) {
        syslog(LOG_ERR, "couldn't set lock on %s: %s!", LOCKFILE, strerror(errno));
        close(fd);
        exit(1);
    }

    syslog(LOG_WARNING, "Writing PID");

    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long) getpid());
    write(fd, buf, strlen(buf) + 1);

    syslog(LOG_WARNING, "PID is written");

    return 0;
}

void daemonize(const char *cmd) {
    int fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;

    umask(0);

    if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        perror("Couldn't get max descriptor number\n");
    }

    if ((pid = fork()) < 0) {
        perror("fork error\n");
    } else if (pid != 0) {
        exit(0);
    }

    if (setsid() == -1) {
        perror("setsid error\n");
    }

    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0) {
        perror("Couldn't ignore SIGHUP\n");
    }

    if (chdir("/") < 0) {
        perror("chdir error\n");
    }

    if (rl.rlim_max == RLIM_INFINITY) {
        rl.rlim_max = 1024;
    }
    for (int i = 0; i < rl.rlim_max; i++) {
        close(i);
    }

    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
        syslog(LOG_ERR, "wrong fds %d %d %d\n", fd0, fd1, fd2);
        exit(1);
    }

    syslog(LOG_WARNING, "Daemonize finished");
}

int main() {
    daemonize("kotyarich");

    if (already_running() != 0) {
        syslog(LOG_ERR, "Daemon is already running\n");
        exit(1);
    }
    syslog(LOG_WARNING, "Check for unique is finished");

    time_t raw_time;
    struct tm *time_info;
    char buf[50];

    while (1) {
        time(&raw_time);
        time_info = localtime(&raw_time);
        sprintf(buf, "Daemon is working, time: %s", asctime(time_info));
        syslog(LOG_INFO, buf);
        sleep(3);
    }
}
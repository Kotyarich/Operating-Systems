#include <bits/types/FILE.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#define BUF_SIZE 1024

void print_environment() {
    char buf[BUF_SIZE];
    size_t len = 0;

    FILE *f = fopen("/proc/self/environ", "r");
    while ((len = fread(buf, 1, BUF_SIZE, f)) > 0) {
        for (size_t i = 0; i < len; i++) {
            if (buf[i] == 0) {
                buf[i] = 10;
            }
        }
        buf[len - 1] = 0;
        printf("%s", buf);
    }
    fclose(f);
}

void print_stat() {
    static char *stat_info[] = {
        "pid", "comm", "state", "ppid", "pgrp", "session"
        "tty_nr", "tpgid", "flags", "minflt", "cminflt",
        "majflt", "cmajflt", "utime", "stime", "cutime",
        "cstime", "priority", "nice", "num_threads",
        "itrealvalue", "starttime", "vsize", "rss", "rsslim",
        "startcode", "endcode", "startstack", "kstkesp",
        "kstkeip", "signal", "blocked", "sigignore",
        "sigcatch", "wchan", "nswap", "cnswap", "exit_signal",
        "processor", "rt_priority", "policy", "delayacct_blkio_ticks",
        "guest_time", "cguest_time", "start_data", "end_data",
        "start_brk", "arg_start", "arg_end", "env_start",
        "end_end", "exit_code", NULL
    };
    char **p_cur_info = &stat_info[0];

    printf("\nstat:\n");
    char buf[BUF_SIZE];

    FILE *f = fopen("/proc/self/stat", "r");
    fread(buf, 1, BUF_SIZE, f);
    char *pch = strtok(buf, " ");

    printf("stat: \n");
    while (pch != NULL) {
        printf("%s: %s\n", *(p_cur_info++), pch);
        pch = strtok(NULL, " ");
    }

    fclose(f);
}

void print_cmdline() {
    printf("\ncmdline:\n");
    char buf[BUF_SIZE];
    size_t len = 0;

    FILE *f = fopen("/proc/self/cmdline", "r");
    while ((len = fread(buf, 1, BUF_SIZE, f)) > 0) {
        for (size_t i = 0; i < len; i++) {
            if (buf[i] == 0) {
                buf[i] = 32;
            }
        }
        buf[len - 1] = 0;
        printf("%s", buf);
    }
    fclose(f);
}

void read_fd() {
    printf("\nfd:\n");
    struct dirent *dirp;
    DIR *dp;

    char str[BUF_SIZE];
    char path[BUF_SIZE];

    dp = opendir("/proc/self/fd");
    while ((dirp = readdir(dp)) != NULL) {
        if ((strcmp(dirp->d_name, ".") != 0) &&
            (strcmp(dirp->d_name, "..") != 0)) {
            sprintf(path, "%s%s", "/proc/self/fd/", dirp->d_name);
            readlink(path, str, BUF_SIZE);
            printf("%s -> %s\n", dirp->d_name, str);
        }
    }
    closedir(dp);
}

int main() {
    print_environment();
    print_stat();
    print_cmdline();
    read_fd();
    return 0;
}

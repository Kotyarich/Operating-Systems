#include <fcntl.h>

int main() {
    char c;
    // have kernel open two connection to file alphabet.txt
    int fd1 = open("alphabet.txt",O_RDONLY);
    int fd2 = open("alphabet.txt",O_RDONLY);
    // read a char & write it alternately from connections fs1 & fd2
    int flag1 = 1;
    int flag2 = 1;
    while(flag1 && flag2) {
        flag1 = read(fd1,&c,1);
        if (flag1 == 1) {
            write(1,&c,1);
        }

        flag2 = read(fd2,&c,1);
        if (flag1 && flag2 == 1) {
            write(1,&c,1);
        }
    }

    return 0;
}
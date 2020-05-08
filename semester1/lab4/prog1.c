#include <stdio.h>
#include <stdlib.h>

int main() {
	int childpid1 = fork();
	if (childpid1 == -1) {
		perror("Can't fork");
		exit(1);
	} else if (childpid1 == 0) {
		printf("Child: pid=%d, pidid=%d, groupid=%d\n", getpid(), getppid(), getpgrp());
		sleep(2);
		printf("Child: pid=%d, pidid=%d, groupid=%d\n", getpid(), getppid(), getpgrp());
		return 0;
	}
	printf("Parent: pid=%d, childpid=%d, groupid=%d\n", getpid(), childpid1, getpgrp());

	int childpid2 = fork();
	if (childpid2 == -1) {
		perror("Can't fork");
		exit(1);
	} else if (childpid2 == 0) {
	        printf("Child: pid=%d, pidid=%d, groupid=%d\n", getpid(), getppid(), getpgrp());
		sleep(2);
		printf("Child: pid=%d, pidid=%d, groupid=%d\n", getpid(), getppid(), getpgrp());
        	return 0;
	}
	printf("Parent: pid=%d, childpid=%d, groupid=%d\n", getpid(), childpid2, getpgrp());
	return 0;
}

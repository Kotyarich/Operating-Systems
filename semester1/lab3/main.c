#include <stdio.h>

int main() {
	int childpid, pearentpid;
	if ((childpid = fork())== -1) {
		perror("Can't fork.\n");
		return 1;
	} else if (childpid == 0) {
		while (1) printf(" childpid = %d ", getpid());
		return 0;
	} else {
		while(1) printf(" pearentpid = %d ", getpid());
		return 0;
	}
} 

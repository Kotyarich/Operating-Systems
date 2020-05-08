#include <stdio.h> //printf
#include <stdlib.h> //exit
#include <unistd.h> //pipe
#include <string.h> //strlen
#include <signal.h>
#include <time.h>

int parent_flag = 0;

void sigint_catcher(int signum) {
	printf("Proccess catched signal #%d\n", signum);
	parent_flag = 1;
}

int main() {
	int descr[2];
	if (pipe(descr) == -1) {
        	perror("Couldn't pipe.");
		exit(1);
	}

	signal(SIGINT, sigint_catcher);

	int child1 = fork();
	if (child1 == -1) {
        	perror("Couldn't fork.");
		exit(1);
	} else if (child1 == 0) {
		close(descr[1]);

		char msg[64];
		memset(msg, 0, 64);
		int i = 0;
		
		printf("Child: reading\n");
		while(read(descr[0], &(msg[i++]), 1) != '\0') ;
		printf("Child: read \"%s\"\n", msg);
	} else {
		close(descr[0]);

		printf("Parent: waiting for CTRL+C signal for 3 seconds...\n");
		sleep(3);

		if (parent_flag) {
		     char msg[64] = "Signal was catched.";
		     write(descr[1], msg, strlen(msg));
		} else {
		     char msg[64] = "Signal wasn't catched";
		     write(descr[1], msg, strlen(msg));
		}
	}
	return 0;
}

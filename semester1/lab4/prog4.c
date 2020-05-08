#include <stdio.h> //printf
#include <stdlib.h> //exit
#include <unistd.h> //pipe
#include <string.h> //strlen


int main() {
	int descr[2];
	if (pipe(descr) == -1) {
	        perror( "Couldn't pipe." );
		exit(1);
	}

	int child1 = fork();
	if (child1 == -1) {
	        perror("Couldn't fork.");
		exit(1);
	} else if (child1 == 0) {
	        close( descr[0] );
        	char msg1[] = "hello1   ";
	        write(descr[1], msg1, 64);
		exit(0);
	}

	int child2 = fork();
	if (child2 == -1) {
	        perror( "Couldn't fork." );
        	exit(1);
	} else if (child2 == 0) {
        	close( descr[0] );
	        char msg2[] = "hello2   ";
	        write(descr[1], msg2, 64);
	        exit(0);
	}
	
	if (child1 != 0 && child2 != 0) {
		close(descr[1]);
		char msg1[64];
		read(descr[0], msg1, 64);

		char msg2[64];
		read(descr[0], msg2, 64);

		printf("Parent: reads %s %s\n", msg1, msg2);

		int status1, status2;
		pid_t ret_value1 = wait(&status1);
		if (WIFEXITED(status1))
		    printf("Parent: child %d finished with %d code.\n\n", ret_value1, WEXITSTATUS(status1));
		else if (WIFSIGNALED(status1))
		    printf("Parent: child %d finished from signal with %d code.\n\n", ret_value1, WTERMSIG(status1));
		else if (WIFSTOPPED(status1))
			printf("Parent: child %d finished from signal with %d code.\n\n", ret_value1, WSTOPSIG(status1));
			
		pid_t ret_value2 = wait(&status2);
		if (WIFEXITED(status2))
			printf("Parent: child %d finished with %d code.\n\n", ret_value2, WEXITSTATUS(status2));
		else if (WIFSIGNALED(status2))
			printf( "Parent: child %d finished from signal with %d code.\n\n", ret_value2, WTERMSIG(status2));
		else if (WIFSTOPPED(status2))
			printf("Parent: child %d finished from signal with %d code.\n\n", ret_value2, WSTOPSIG(status2));
	}

	return 0;
}

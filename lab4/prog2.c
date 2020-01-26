#include <stdio.h> //printf
#include <stdlib.h> //exit
#include <sys/types.h> //W***
#include <sys/wait.h> //wait


int main() {
	int child1 = fork();
	int child2;
	if ( child1 == -1 ) {
		perror("Couldn't fork.");
		exit(1);
	} else if ( child1 == 0 ) {
		sleep(2);
		printf( "Child: pid=%d; group=%d; parent=%d\n", getpid(), getpgrp(), getppid() );
		return 0;
	}
	printf( "Parent: pid=%d; group=%d; child=%d\n", getpid(), getpgrp(), child1 );
	
	int status1;
	pid_t ret_value1 = wait(&status1);
	if (WIFEXITED(status1))
		printf("Parent: child %d finished with %d code.\n", ret_value1, WEXITSTATUS(status1) );
	else if (WIFSIGNALED(status1))
		printf( "Parent: child %d finished from signal with %d code.\n", ret_value1, WTERMSIG(status1));
	else if (WIFSTOPPED(status1))
		printf("Parent: child %d finished from signal with %d code.\n", ret_value1, WSTOPSIG(status1));

	child2 = fork();
	if ( child2 == -1 ) {
        	perror("Couldn't fork.");
                exit(1);
	} else if ( child2 == 0 ) {
	        printf( "Child: pid=%d; group=%d; parent=%d\n", getpid(), getpgrp(), getppid() );
		sleep(2);
		return 0;
        }
	printf( "Parent: pid=%d; group=%d; child=%d\n", getpid(), getpgrp(), child2 );
        
	int status2;
        pid_t ret_value2 = wait(&status2);
        if (WIFEXITED(status2))
        	printf("Parent: child %d finished with %d code.\n", ret_value2, WEXITSTATUS(status2) );
	else if (WIFSIGNALED(status2))
        	printf( "Parent: child %d finished from signal with %d code.\n", ret_value2, WTERMSIG(status2));
        else if (WIFSTOPPED(status2))
	        printf("Parent: child %d finished from signal with %d code.\n", ret_value2, WSTOPSIG(status2));
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
	printf("hello world (pid:%d)\n", (int)getpid());
	int rc = fork();
	if (rc < 0) { // fork failed; exit
		fprintf(stderr, "fork failed\n");
		exit(EXIT_FAILURE);
	} else if (rc == 0) { // child (new process)
		printf("hello, I am child (pid:%d)\n", (int)getpid());
	} else { // parent goes down this path (main)
		int wc = wait(NULL);
		printf("hello, I am parent of %d (wc:%d) (pid:%d)\n", rc, wc,
		       (int)getpid());
	}
	return 0;
}

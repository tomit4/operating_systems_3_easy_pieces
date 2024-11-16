#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
	int rc = fork();
	if (rc < 0) { // fork failed; exit
		fprintf(stderr, "fork failed\n");
		exit(EXIT_FAILURE);
	} else if (rc == 0) { // child (new process)
		close(STDOUT_FILENO);
		open("./p4.output", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
		// now exec "wc"...
		char *myargs[3];
		myargs[0] = strdup("wc");   // program: "wc" (word count)
		myargs[1] = strdup("p3.c"); // argument: file to count
		myargs[2] = NULL;	    // marks end of array
		execvp(myargs[0], myargs);  // runs word count
	} else {			    // parent goes down this path (main)
		wait(NULL);
	}
	return 0;
}

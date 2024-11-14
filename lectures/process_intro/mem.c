#include "common.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int value;

// to turn off memory randomization, run compiled mem with lldb:
// `lldb mem`

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "usage: mem <value>\n");
		exit(1);
	}
	int *p;	    // memory for pointer is on "stack"
	p = &value; // p contains address of heap variable

	printf("(pid:%d) addr stored in p: %llx\n", (int)getpid(),
	       (unsigned long long)p);
	*p = atoi(argv[1]); // assign value stored in p
	while (1) {
		Spin(1);
		*p = *p + 1;
		printf("(pid:%d) value of p: %d\n", getpid(), *p);
	}
	return 0;
}

#include "../../common.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int value = 10;

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "usage: mem <value>\n");
		exit(1);
	}
	printf("{pid:%d} initial value in : %d\n", (int)getpid(), value);
	value = atoi(argv[1]); // assign value to argument
	while (1) {
		Spin(1);
		printf("{pid:%d} value of p: %d\n", getpid(), value);
	}
	return 0;
}

#include "../../common.h"
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile int value = 10;

void *worker(void *myarg)
{
	int myvalue = atoi((char *)myarg);
	printf("{pid:%d, tid:%d} Initial value of p: %d\n", getpid(),
	       (int)pthread_self(), value);
	value = myvalue;

	while (1) {
		printf("{pid:%d, tid:%d} value of p: %d\n", getpid(),
		       (int)pthread_self(), value);
		Spin(1);
	}
}

int main(int argc, char *argv[])
{
	if (argc != 3) {
		fprintf(stderr, "usage: threads <value1> <value2>\n");
		exit(1);
	}

	pthread_t p1, p2;
	printf("Initial value : %d\n", value);
	pthread_create(&p1, NULL, worker, argv[1]);
	sleep(1);
	pthread_create(&p2, NULL, worker, argv[2]);
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);

	return 0;
}

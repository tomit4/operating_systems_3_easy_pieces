CC=gcc
CFLAGS=-Wall -Wextra -Werror -std=c99
FILE=../../../bin/process_api/process

process:
	$(CC) -o $(FILE) process.c $(CFLAGS)

execute:
	$(FILE) 11

clean:
	rm -f $(FILE)

CC=cc
CFLAGS=-Werror -Wall -g -std=c99

dscheduler: *.c
	$(CC) $(CFLAGS) -o dscheduler *.c

test: dscheduler
	@bash ./test.sh

clean:
	rm dscheduler

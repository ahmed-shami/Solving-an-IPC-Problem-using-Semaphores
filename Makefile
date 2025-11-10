CC=gcc
CFLAGS=-Wall -Wextra -O2 -pthread

all: example shm_processes bank_basic ref_psdd

example: example.c
	$(CC) $(CFLAGS) -o example example.c

shm_processes: shm_processes.c
	$(CC) $(CFLAGS) -o shm_processes shm_processes.c

bank_basic: bank_basic.c
	$(CC) $(CFLAGS) -o bank_basic bank_basic.c

ref_psdd: ref_psdd.c
	$(CC) $(CFLAGS) -o ref_psdd ref_psdd.c

clean:
	rm -f example shm_processes bank_basic ref_psdd bank.mm bank_ex.mm log.txt

#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

int main(int argc, char **argv)
{
  int fd, i, nloop = 10, zero = 0, *counter_ptr;
  sem_t *mutex;

  fd = open("log.txt", O_RDWR | O_CREAT, S_IRWXU);
  if (fd < 0) { perror("open"); exit(1); }
  if (write(fd, &zero, sizeof(int)) != sizeof(int)) { perror("write"); exit(1); }

  counter_ptr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (counter_ptr == MAP_FAILED) { perror("mmap"); exit(1); }
  close(fd);

  if ((mutex = sem_open("examplesemaphore", O_CREAT, 0644, 1)) == SEM_FAILED) {
    perror("semaphore initialization");
    exit(1);
  }

  if (fork() == 0) { /* child */
    for (i = 0; i < nloop; i++) {
      printf("child wanting to enter critical section\n");
      sem_wait(mutex);
      printf("child entered critical section: %d\n", (*counter_ptr)++);
      sleep(2);
      printf("child leaving critical section\n");
      sem_post(mutex);
      sleep(1);
    }
    exit(0);
  }

  for (i = 0; i < nloop; i++) {
    printf("parent wanting to enter critical section\n");
    sem_wait(mutex);
    printf("parent entered critical section: %d\n", (*counter_ptr)++);
    sleep(2);
    printf("parent leaving critical section\n");
    sem_post(mutex);
    sleep(1);
  }
  exit(0);
}

#define _XOPEN_SOURCE 700
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

static volatile sig_atomic_t running = 1;
static sem_t *mutex = NULL;
static int *BankAccount = NULL;

static void stop_handler(int sig) { (void)sig; running = 0; }

static void seed_rng(void) {
    unsigned s = (unsigned)time(NULL) ^ (unsigned)getpid();
    srand(s);
}

static void die(const char *msg) { perror(msg); exit(1); }

static void open_shared_balance(void) {
    int fd = open("bank.mm", O_RDWR | O_CREAT, 0600);
    if (fd < 0) die("open bank.mm");
    int zero = 0;
    if (ftruncate(fd, sizeof(int)) < 0) die("ftruncate bank.mm");
    if (write(fd, &zero, sizeof(int)) != sizeof(int)) die("write bank.mm");
    BankAccount = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (BankAccount == MAP_FAILED) die("mmap bank");
    close(fd);
}

static void open_semaphore(void) {
    mutex = sem_open("/bank_mutex_sem", O_CREAT, 0644, 1);
    if (mutex == SEM_FAILED) die("sem_open");
}

static void cleanup_parent(void) {
    if (BankAccount && BankAccount != MAP_FAILED) munmap((void*)BankAccount, sizeof(int));
    if (mutex && mutex != SEM_FAILED) sem_close(mutex);
    /* Do not unlink here while a child may still run. SIGINT will end both. */
}

static void dad_loop(void) {
    seed_rng();
    while (running) {
        sleep(rand() % 6);
        printf("Dear Old Dad: Attempting to Check Balance\n");
        sem_wait(mutex);

        int localBalance = *BankAccount;

        if ((rand() % 2) == 0) {
            if (localBalance < 100) {
                int amount = rand() % 101; /* 0..100 */
                if ((rand() % 2) == 0) {
                    localBalance += amount;
                    *BankAccount = localBalance;
                    printf("Dear Old Dad: Deposits $%d / Balance = $%d\n", amount, localBalance);
                } else {
                    printf("Dear Old Dad: Doesn't have any money to give\n");
                }
            } else {
                printf("Dear Old Dad: Thinks Student has enough Cash ($%d)\n", localBalance);
            }
        } else {
            printf("Dear Old Dad: Last Checking Balance = $%d\n", localBalance);
        }

        sem_post(mutex);
        fflush(stdout);
    }
}

static void student_loop(void) {
    seed_rng();
    while (running) {
        sleep(rand() % 6);
        printf("Poor Student: Attempting to Check Balance\n");
        sem_wait(mutex);

        int localBalance = *BankAccount;

        if ((rand() % 2) == 0) {
            int need = rand() % 51; /* 0..50 */
            printf("Poor Student needs $%d\n", need);
            if (need <= localBalance) {
                localBalance -= need;
                *BankAccount = localBalance;
                printf("Poor Student: Withdraws $%d / Balance = $%d\n", need, localBalance);
            } else {
                printf("Poor Student: Not Enough Cash ($%d)\n", localBalance);
            }
        } else {
            printf("Poor Student: Last Checking Balance = $%d\n", localBalance);
        }

        sem_post(mutex);
        fflush(stdout);
    }
}

int main(void) {
    signal(SIGINT, stop_handler);
    signal(SIGTERM, stop_handler);

    open_shared_balance();
    open_semaphore();

    pid_t child = fork();
    if (child < 0) die("fork");
    if (child == 0) {
        student_loop();
        _exit(0);
    } else {
        dad_loop();
        kill(child, SIGTERM);
        wait(NULL);
        cleanup_parent();
        /* After both exit, safe to unlink the named semaphore */
        sem_unlink("/bank_mutex_sem");
    }
    return 0;
}

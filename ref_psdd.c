#define _XOPEN_SOURCE 700
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

static volatile sig_atomic_t running = 1;

typedef struct {
    int balance;
} SharedState;

static SharedState *S = NULL;
static sem_t *mutex = NULL;

static void stop_handler(int sig) { (void)sig; running = 0; }

static void seed_rng(void) {
    unsigned s = (unsigned)time(NULL) ^ (unsigned)getpid();
    srand(s);
}

static void die(const char *msg) { perror(msg); exit(1); }

static void setup_shared(void) {
    int fd = open("bank_ex.mm", O_RDWR | O_CREAT, 0600);
    if (fd < 0) die("open bank_ex.mm");
    if (ftruncate(fd, sizeof(SharedState)) < 0) die("ftruncate");
    if (write(fd, &(SharedState){0}, sizeof(SharedState)) != sizeof(SharedState)) die("write");
    S = mmap(NULL, sizeof(SharedState), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (S == MAP_FAILED) die("mmap");
    close(fd);
}

static void setup_sem(void) {
    mutex = sem_open("/psdd_mutex_sem", O_CREAT, 0644, 1);
    if (mutex == SEM_FAILED) die("sem_open");
}

static void loop_dad(void) {
    seed_rng();
    while (running) {
        sleep(rand() % 6);
        printf("Dear Old Dad: Attempting to Check Balance\n");
        sem_wait(mutex);
        int local = S->balance;

        if ((rand() % 2) == 0) {
            if (local < 100) {
                int amount = rand() % 101; /* 0..100 */
                if ((rand() % 2) == 0) {
                    local += amount;
                    S->balance = local;
                    printf("Dear Old Dad: Deposits $%d / Balance = $%d\n", amount, local);
                } else {
                    printf("Dear Old Dad: Doesn't have any money to give\n");
                }
            } else {
                printf("Dear Old Dad: Thinks Student has enough Cash ($%d)\n", local);
            }
        } else {
            printf("Dear Old Dad: Last Checking Balance = $%d\n", local);
        }
        sem_post(mutex);
        fflush(stdout);
    }
}

static void loop_mom(void) {
    seed_rng();
    while (running) {
        sleep(rand() % 11); /* 0..10 seconds */
        printf("Lovable Mom: Attempting to Check Balance\n");
        sem_wait(mutex);
        int local = S->balance;
        if (local <= 100) {
            int amount = 25 + (rand() % 101); /* 25..125 */
            local += amount;
            S->balance = local;
            printf("Lovable Mom: Deposits $%d / Balance = $%d\n", amount, local);
        } else {
            printf("Lovable Mom: Balance already healthy ($%d)\n", local);
        }
        sem_post(mutex);
        fflush(stdout);
    }
}

static void loop_student(int idx) {
    seed_rng();
    while (running) {
        sleep(rand() % 6);
        printf("Poor Student(%d): Attempting to Check Balance\n", idx);
        sem_wait(mutex);
        int local = S->balance;

        if ((rand() % 2) == 0) {
            int need = rand() % 51; /* 0..50 */
            printf("Poor Student(%d) needs $%d\n", idx, need);
            if (need <= local) {
                local -= need;
                S->balance = local;
                printf("Poor Student(%d): Withdraws $%d / Balance = $%d\n", idx, need, local);
            } else {
                printf("Poor Student(%d): Not Enough Cash ($%d)\n", idx, local);
            }
        } else {
            printf("Poor Student(%d): Last Checking Balance = $%d\n", idx, local);
        }
        sem_post(mutex);
        fflush(stdout);
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <parents: 1=dad, 2=mom+dad> <num_students>\n", argv[0]);
        return 1;
    }
    int parent_mode = atoi(argv[1]);   /* 1 or 2 */
    int n_students  = atoi(argv[2]);   /* >= 1 */

    signal(SIGINT, stop_handler);
    signal(SIGTERM, stop_handler);

    setup_shared();
    setup_sem();

    pid_t *pids = calloc((size_t)(2 + n_students), sizeof(pid_t));
    int pcount = 0;

    /* Dad always */
    pid_t p = fork();
    if (p < 0) die("fork dad");
    if (p == 0) { 
        loop_dad(); 
        _exit(0); 
    }
    pids[pcount++] = p;

    /* Mom if requested */
    if (parent_mode >= 2) {
        p = fork();
        if (p < 0) die("fork mom");
        if (p == 0) { 
            loop_mom(); 
            _exit(0); 
        }
        pids[pcount++] = p;
    }

    /* Students */
    for (int i = 1; i <= n_students; i++) {
        p = fork();
        if (p < 0) die("fork student");
        if (p == 0) { 
            loop_student(i); 
            _exit(0); 
        }
        pids[pcount++] = p;
    }

    /* Parent waits for signal, then shuts everyone down */
    while (running) pause();

    for (int i = 0; i < pcount; i++) kill(pids[i], SIGTERM);
    for (int i = 0; i < pcount; i++) waitpid(pids[i], NULL, 0);

    munmap(S, sizeof(SharedState));
    sem_close(mutex);
    sem_unlink("/psdd_mutex_sem");
    free(pids);
    return 0;
}

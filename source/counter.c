#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>

#define TARGET 1000000000L
#define MIN 2
#define MED 4
#define MAX 8

#define SEM_NAME "/counter_t1_sem"

// Experimento T1: Threads sem sincronizacao
static long counter_t1 = 0;

static void *worker_t1(void *arg)
{
    long limit = *(long *)arg;
    while (counter_t1 < limit)
        counter_t1++;
    return NULL;
}

void experiment_t1(int n)
{
    pthread_t threads[n];
    long limit = TARGET;
    counter_t1 = 0;

    for (int i = 0; i < n; i++)
        if (pthread_create(&threads[i], NULL, worker_t1, &limit)) {
            perror("pthread_create"); exit(1);
        }

    for (int i = 0; i < n; i++)
        pthread_join(threads[i], NULL);

    printf("T1 (N=%d): counter = %ld\n", n, counter_t1);
}

// Experimento T2: Threads com pthread_mutex
static long counter_t2 = 0;
static pthread_mutex_t mutex_t2 = PTHREAD_MUTEX_INITIALIZER;

static void *worker_t2(void *arg)
{
    long limit = *(long *)arg;
    while (1) {
        pthread_mutex_lock(&mutex_t2);
        if (counter_t2 >= limit) {
            pthread_mutex_unlock(&mutex_t2);
            break;
        }
        counter_t2++;
        pthread_mutex_unlock(&mutex_t2);
    }
    return NULL;
}

void experiment_t2(int n)
{
    pthread_t threads[n];
    long limit = TARGET;
    counter_t2 = 0;

    for (int i = 0; i < n; i++)
        if (pthread_create(&threads[i], NULL, worker_t2, &limit)) {
            perror("pthread_create"); exit(1);
        }

    for (int i = 0; i < n; i++)
        pthread_join(threads[i], NULL);

    printf("T2 (N=%d): counter = %ld\n", n, counter_t2);
}

// Experimento P1: Processos com memoria compartilhada, sem sincronizacao
void experiment_p1(int n)
{
    int shmid = shmget(IPC_PRIVATE, sizeof(long), IPC_CREAT | 0666);
    if (shmid < 0) { perror("shmget"); exit(1); }

    long *counter = (long *)shmat(shmid, NULL, 0);
    if (counter == (long *)-1) { perror("shmat"); exit(1); }
    *counter = 0;

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid < 0) { perror("fork"); exit(1); }
        if (pid == 0) {
            while (*counter < TARGET)
                (*counter)++;
            shmdt(counter);
            exit(0);
        }
    }

    for (int i = 0; i < n; i++)
        wait(NULL);

    printf("P1 (N=%d): counter = %ld\n", n, *counter);

    shmdt(counter);
    shmctl(shmid, IPC_RMID, NULL);
}

// Experimento P2: Processos com memoria compartilhada + semaforos
void experiment_p2(int n)
{
    sem_unlink(SEM_NAME);
    sem_t *sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0666, 1);
    if (sem == SEM_FAILED) { perror("sem_open"); exit(1); }

    int shmid = shmget(IPC_PRIVATE, sizeof(long), IPC_CREAT | 0666);
    if (shmid < 0) { perror("shmget"); exit(1); }

    long *counter = (long *)shmat(shmid, NULL, 0);
    if (counter == (long *)-1) { perror("shmat"); exit(1); }
    *counter = 0;

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid < 0) { perror("fork"); exit(1); }
        if (pid == 0) {
            while (1) {
                sem_wait(sem);
                if (*counter >= TARGET) {
                    sem_post(sem);
                    break;
                }
                (*counter)++;
                sem_post(sem);
            }
            exit(0);
        }
    }

    for (int i = 0; i < n; i++)
        wait(NULL);

    printf("P2 (N=%d): counter = %ld\n", n, *counter);

    shmdt(counter);
    shmctl(shmid, IPC_RMID, NULL);
    sem_close(sem);
    sem_unlink(SEM_NAME);
}

// Main
int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <T1|T2|P1|P2> <2|4|8>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[2]);
    if (n != MIN && n != MED && n != MAX) {
        fprintf(stderr, "N deve ser 2, 4 ou 8.\n");
        return 1;
    }

    if      (strcmp(argv[1], "T1") == 0) experiment_t1(n);
    else if (strcmp(argv[1], "T2") == 0) experiment_t2(n);
    else if (strcmp(argv[1], "P1") == 0) experiment_p1(n);
    else if (strcmp(argv[1], "P2") == 0) experiment_p2(n);
    else {
        fprintf(stderr, "Experimento invalido: %s\n", argv[1]);
        return 1;
    }

    return 0;
}

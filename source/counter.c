#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Definição das quantidades bases estabelecidas pelo enunciado do trabalho.
#define MIN 2
#define MED 4
#define MAX 8

// Init
void *threadFormiga(void *arg);
int counter = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int main()
{
    pthread_t trabalhadoresMin[MIN];
    pthread_t trabalhadoresMed[MED];
    pthread_t trabalhadoresMax[MAX];

    int ret;

    int target = 100;

    for(int i = 0; i < MIN; i++)
    {
        ret = pthread_create(&trabalhadoresMin[i], NULL, threadFormiga, &target);
        if(ret)
        {
            printf("Erro ao criar a thread %d\n", i);
            exit(-1);
        }
    }

    for (int i = 0; i < MIN; i++)
    {
        pthread_join(trabalhadoresMin[i], NULL);
    }

    printf("Estado atual do counter : %d \n", counter);
    return 0;
}

void *threadFormiga(void *arg)
{
    int target = *((int *)arg);
    
    for(int i = 0; i < target; i++)
    {
        pthread_mutex_lock(&lock);
        counter+=1;
        pthread_mutex_unlock(&lock);
    }

    return NULL;
}

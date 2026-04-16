#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Definição das quantidades bases estabelecidas pelo enunciado do trabalho.
#define MIN 2
#define MED 4
#define MAX 8

void *threadFormiga(void *arg);

int counter = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int main()
{
    pthread_t meuTrabalhador;
    int ret;

    printf("Estado inicial Counter : %d \n", counter);
    printf("Criando nova thread trabalhadora :\n");
    ret = pthread_create(&meuTrabalhador, NULL, &threadFormiga, &counter);

    if(ret != 0)
    {
        printf("Erro ao criar o novo trabalhador!\n");
        exit(EXIT_FAILURE);
    }
    printf("Estado atual do counter : %d \n", counter);
    pthread_exit(NULL);
}

void *threadFormiga(void *arg)
{
    printf("Trabalhador criado!\n");
    pthread_mutex_lock(&lock);
    counter+=1;
    pthread_mutex_unlock(&lock);
    return NULL;
}

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Definição das quantidades bases estabelecidas pelo enunciado do trabalho.
#define MIN 2
#define MED 4
#define MAX 8

void *threadFormiga(void *arg);

int counter;

int main()
{
    //byThreads(MIN); byThreads(MED); byThreads(MAX);
    //byFork(MIN); byFork(MED); byFork(MAX);

    pthread_t meuTrabalhador;
    int ret;

    printf("Criando nova thread trabalhadora :\n");
    ret = pthread_create(&meuTrabalhador, NULL, &threadFormiga, NULL);

    if(ret != 0)
    {
        printf("Erro ao criar o novo trabalhador!\n");
        exit(EXIT_FAILURE);
    }
    pthread_exit(NULL);
}

//int byThreads(int numberThreads)
//{
    // Limpa o contador, só pra ter certeza.
//    counter = 0;
//}

void *threadFormiga(void *arg)
{
    printf("Trabalhador criado!\n");
}

//int byFork(int numberProcess)
//{
    // Limpa o contador, só pra ter certeza.
//    counter = 0;
//}

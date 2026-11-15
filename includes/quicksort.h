#ifndef QUICKSORT_H
#define QUICKSORT_H

#include <stdio.h>
#include <stdlib.h>

#define LIMITE_MEMORIA 10

typedef struct{
    long int inscrica[8];
    float nota[5];
    char estado[3];
    char cidade[51];
    char curso[31];
}Item;


void inserePivo(FILE* arq, int *transEscrita, Item pivo[], int N, long EscritaEsq );
int removeMaiorPivo(FILE* arq, Item pivo[], int* N, int *transEscrita, long* EscritaDir, float* Lsup);
int gravaPivo(FILE* arq, Item pivo[], Int N, long EscritaEsq, int* transEscrita);
int particiona(FILE* arq, long Esq, long Dir, int* i, long* j, int* transEscrita, int* transLeitura, int* comp);
void quicksortRec(FILE* arq, long Esq, long Dir, int* transEscrita, int* transLeitura , int* comp);
//Adicionar o resto das funções (stephany)


#endif
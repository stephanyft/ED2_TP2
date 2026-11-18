#ifndef QUICKSORT_H
#define QUICKSORT_H

#include <stdio.h>
#include <stdlib.h>

#define LIMITE_MEMORIA 10

typedef struct {
    long leituras;
    long escritas;
    long comparacoes;
} Estatisticas;
typedef struct{
    long int inscricao;
    float nota;
    char estado[3];
    char cidade[51];
    char curso[31];
}Registro;

int leRegistro(FILE* arq, long r, Registro* reg, Estatisticas* est);
int escreveRegistro(FILE* arq, long r, Registro* reg, Estatisticas* est);

void inserePivo(FILE* arq, Estatisticas* est, Registro pivo[], int* N, long* EscritaEsq, Registro reg);
int removeMenorPivo(FILE* arq, Registro pivo[], int* N, Estatisticas* est, long* EscritaEsq, float* Linf);
int removeMaiorPivo(FILE* arq, Registro pivo[], int* N, Estatisticas* est, long* EscritaDir, float* Lsup);
int gravaPivo(FILE* arq, Registro pivo[], int N, long EscritaEsq, Estatisticas* est);

void quicksort(FILE* arq, long n, Estatisticas* est);
int particiona(FILE* arq, long Esq, long Dir, long* i, long* j, Estatisticas* est);
void quicksortRec(FILE* arq, long Esq, long Dir, Estatisticas* est);


#endif
#ifndef QUICKSORT_H
#define QUICKSORT_H

#include <stdio.h>
#include <stdlib.h>
#include "registro.h"
#include "estatisticas.h"

#define LIMITE_MEMORIA 10

int leRegistro(FILE* arq, long r, Registro* reg, Estatisticas* est);
int escreveRegistro(FILE* arq, long r, Registro* reg, Estatisticas* est);

void inserePivo(Estatisticas* est, Registro pivo[], int* N, Registro reg);
int removeMenorPivo(FILE* arq, Registro pivo[], int* N, Estatisticas* est, long* EscritaEsq, float* Linf);
int removeMaiorPivo(FILE* arq, Registro pivo[], int* N, Estatisticas* est, long* EscritaDir, float* Lsup);
int gravaPivo(FILE* arq, Registro pivo[], int N, long EscritaEsq, Estatisticas* est);


int particiona(FILE* arq, long Esq, long Dir, long* i, long* j, Estatisticas* est);
void quicksort(FILE* arq, long n, Estatisticas* est);
void quicksortRec(FILE* arq, long Esq, long Dir, Estatisticas* est);
int metodoQuicksortExterno(FILE* txt, int n, int imprimir, Estatisticas* est);

#endif
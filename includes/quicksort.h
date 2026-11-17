
#ifndef QUICKSORT_H
#define QUICKSORT_H

#include <stdio.h>
#include "registros.h"

#define LIMITE_MEMORIA 10


int inserePivo(Registro pivo[], Registro lido,int *N, int *comparacoes);
int gravaPivo(FILE *arq, Registro pivo[],int N, long EscritaEsq,int *transfEscrita);
int removeMaiorPivo(FILE *arq, Registro pivo[],int *N, int *transfEscrita,long *EscritaDir, float *Lsup);
int particiona(FILE *arq, long Esq, long Dir,long *i, long *j, int *transfLeitura, int *transfEscrita,int *comparacoes);
void quicksortRec(FILE *arq, long Esq, long Dir,int *transfLeitura, int *transfEscrita,int *comparacoes);


#endif

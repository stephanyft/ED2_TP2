#ifndef QUICKSORT_H
#define QUICKSORT_H

#include <stdio.h>
#include <stdlib.h>
#include "registro.h"
#include "estatisticas.h"

#define LIMITE_MEMORIA 10

// Funções responsáveis pela leitura e escrita de registros no arquivo binário.
int leRegistro(FILE* arq, long r, Registro* reg, Estatisticas* est);
int escreveRegistro(FILE* arq, long r, Registro* reg, Estatisticas* est);

// Insere registros no pivô mantendo a ordenação por nota.
void inserePivo(Estatisticas* est, Registro pivo[], int* N, Registro reg);

// Remove o menor registro do pivô e o grava na região esquerda do arquivo.
int removeMenorPivo(FILE* arq, Registro pivo[], int* N, Estatisticas* est, long* EscritaEsq, float* Linf);

// Remove o maior registro do pivô e o grava na região direita do arquivo.
int removeMaiorPivo(FILE* arq, Registro pivo[], int* N, Estatisticas* est, long* EscritaDir, float* Lsup);

// Grava no arquivo os registros que permaneceram armazenados no pivô.
int gravaPivo(FILE* arq, Registro pivo[], int N, long EscritaEsq, Estatisticas* est);

// Realiza a partição do arquivo, distribuindo os registros entre as regiões esquerda e direita.
int particiona(FILE* arq, long Esq, long Dir, long* i, long* j, Estatisticas* est);

// Inicia o processo de ordenação do arquivo binário.
void quicksort(FILE* arq, long n, Estatisticas* est);

// Executa recursivamente o Quicksort sobre as partições geradas.
void quicksortRec(FILE* arq, long Esq, long Dir, Estatisticas* est);

// Coordena todo o processo: conversão para binário, ordenação e geração do arquivo final em texto.
int metodoQuicksortExterno(FILE* txt, int n, int imprimir, Estatisticas* est);

#endif

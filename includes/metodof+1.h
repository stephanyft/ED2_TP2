#ifndef INTERCALACAO_F_MAIS_1_H
#define INTERCALACAO_F_MAIS_1_H

#include <stdio.h>
#include "registro.h"
#include "estatisticas.h"
#include "fita.h"

#define F_FITAS 19
#define NUM_FITAS_TOTAL (F_FITAS + 1)
#define TAM_MEMORIA_F1 19

typedef struct {
    Registro reg;
    int marcado;
} ItemSubF1;

typedef  struct {
  ItemSubF1 v[19];
  int tamanho;  
} HeapSubF1;

void metodoFMaisUmCaminhos(FILE *arq, int quantidade, Estatisticas *est);
static int menorSubF1(ItemSubF1 a, ItemSubF1 b, Estatisticas *est);
static void descerHeapSubF1(HeapSubF1 *h, int i, Estatisticas *est);
static void construirHeapSubF1(HeapSubF1 *h, Estatisticas *est);
static ItemSubF1 removerRaizSubF1(HeapSubF1 *h, Estatisticas *est);
static void substituirRaizSubF1( HeapSubF1 * h, Registro novo, Registro ultimo, Estatisticas *est);



#endif

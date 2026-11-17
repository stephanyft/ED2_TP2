#ifndef DOISF_H
#define DOISF_H

#include "../estatisticas.h"
#include "../fita.h"
#include "../subSelecao.h"
#include "../registro.h"
#include <stdio.h>
#include <stdlib.h>

int metodoDoisFCaminhos(FILE *arq, int n, int imprimir, Estatisticas *est);
int ordenaBlocosIniciais(FILE *arq, int n, Fita* f[], Estatisticas *est);
int intercalaFitas(Fita* f[], Estatisticas* est);
void escreveBloco(FILE *arq, Fita *f, Heap *h, int *cont, int *lidos, int n, int *qtdHeap, Estatisticas *est);

#endif //2F_H
#ifndef SUBSELECAO_H
#define SUBSELECAO_H

#include "registro.h"
#include <stdbool.h>

#define TAM_HEAP 10

typedef struct {
    Registro reg;
    bool marcado;
    int fitaOrigem;
}ItemHeap;

typedef struct {
    ItemHeap v[TAM_HEAP];
}Heap;

int menor(ItemHeap a, ItemHeap b, Estatisticas* est);
bool fimBloco(Heap *h, int n);
void preparaHeap(Heap *h, int n, Estatisticas* est);
void novoBloco(Heap *h, int n, Estatisticas* est);
void constroiHeap(Heap *h, int n, Estatisticas* est);
void substituiRaiz(Heap *h, Registro r, Registro ultimo, int n, Estatisticas* est);
void reconstroiHeap(Heap *h, int i, int n, Estatisticas* est);


#endif //SUBSELECAO_H
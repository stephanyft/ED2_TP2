#ifndef SUBSELECAO_H
#define SUBSELECAO_H

#include "registro.h"
#include <stdbool.h>

#define TAM_HEAP 10

typedef struct {
    Registro reg;
    bool marcado;
}ItemHeap;

typedef struct {
    ItemHeap v[10]
}Heap;

int menor(ItemHeap a, ItemHeap b);
bool fimBloco(Heap *h, int n);
void preparaHeap(Heap *h, int n);
void novoBloco(Heap *h, int n);
void constroiHeap(Heap *h, int n);
void substituiRaiz(Heap *h, Registro r, Registro ultimo, int n);
void reconstroiHeap(Heap *h, int i, int n);


#endif //SUBSELECAO_H
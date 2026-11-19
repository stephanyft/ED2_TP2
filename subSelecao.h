#ifndef SUBSELECAO_H
#define SUBSELECAO_H

#include "registro.h"
#include <stdbool.h>

#define TAM_HEAP 19

typedef struct {
    Registro reg;
    bool marcado;
    int fitaOrigem;
}ItemHeap;

typedef struct {
    ItemHeap v[TAM_HEAP];
}Heap;

//Funçao que verifica qual item eh menor
int menor(ItemHeap a, ItemHeap b, Estatisticas* est);

//Funçao que verifica se todos os itens do heap estao marcados
bool fimBloco(Heap *h, int n);

//Funçao que desmarca todos os itens do heap, caso todos estejam marcados
void preparaHeap(Heap *h, int n, Estatisticas* est);

//Funçao que desmarca todos os itens do bloco e o reorganiza
void novoBloco(Heap *h, int n, Estatisticas* est);

//Funçao que transforma o vetor inicial em um heap valido
void constroiHeap(Heap *h, int n, Estatisticas* est);

//Substitui a raiz do heap pelo novo registro, comparando as notas para marcar se nescessario
void substituiRaiz(Heap *h, Registro r, Registro ultimo, int n, Estatisticas* est);

//Funçao que reconstroi o vetor em um heap valido
void reconstroiHeap(Heap *h, int i, int n, Estatisticas* est);


#endif //SUBSELECAO_H
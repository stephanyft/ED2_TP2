#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../includes/metodof+1.h"

typedef struct {
    Registro reg;
    int marcado;
} ItemSubF1;

typedef  struct {
  ItemSubF1 v[19];
  int tamanho;  
} HeapSubF1;


static int menorSubF1(ItemSubF1 a, ItemSubF1 b, Estatisticas *est) {
    if(a.marcado != b.marcado) { 
        return a.marcado < b.marcado;
    }
    return comparaNota(est, &a.reg, &b.reg) < 0;
}
static void descerHeapSubF1(HeapSubF1 *h, int i, Estatisticas *est) {
    int menor = i;
    int esq = 2 * i + 1;
    int dir = 2 * i + 2;

    if(esq < h ->tamanho && menorSubF1(h->v[esq], h->v[menor], est)){
        menor = esq;
    }
    if(dir < h->tamanho && menorSubF1(h->v[dir], h->v[menor], est)){
        menor = dir;
    }

}
static void construirHeapSubF1(HeapSubF1 *h, Estatisticas *est) {
    for(int i = h->tamanho / 2 - 1; i >= 0; i--){
        descerHeapSubF1(h, i, est);
    }
}
static ItemSubF1 removerRaizSubF1(HeapSubF1 *h, Estatisticas *est){
    ItemSubF1 menor = h->v[0];
    h->[0] = h->[h->tamanho - 1];
    h->tamanho--;
    if(h->tamanho > 0) {
        descerHeapSubF1(h, 0, est);
    }

}
 static void substituirRaizSubF1( HeapSubF1 * h, Registro novo, Registro ultimo, Estatisticas *est)
    h->v[0].reg = novo;
    if(comparaNota(est, &novo, &ultimo) < 0) {
        h->v[0].marcado = 1;
    }
    else{
        h->v[0].marcado = 0;

    }
    descerHeapSubF1(h, 0, est);



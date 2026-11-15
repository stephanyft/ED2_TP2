#include "subSelecao.h"

int menor(ItemHeap a, ItemHeap b, Estatisticas* est) {
    est->comparacoes++;

    if (a.marcado != b.marcado)
        return a.marcado < b.marcado;

    return a.reg.nota < b.reg.nota;
}

bool fimBloco(Heap *h, int n) {
    for (int i = 0; i < n; i++) {
        if (!h->v[i].marcado)
            return false;
    }

    return true;
}

void preparaHeap(Heap *h, int n, Estatisticas* est) {
    if (fimBloco(h, n)) {
        novoBloco(h, n, est);
    }
}

void novoBloco(Heap *h, int n, Estatisticas* est) {
    for (int i = 0; i < n; i++) {
        h->v[i].marcado = false;
    }

    constroiHeap(h, n, est);
}

void constroiHeap(Heap *h, int n, Estatisticas* est) {
    for (int i = n / 2 - 1; i >= 0; i--) {
        reconstroiHeap(h, i, n, est);
    }
}

void substituiRaiz(Heap *h, Registro r, Registro ultimo, int n, Estatisticas* est) {
    h->v[0].reg = r;
    h->v[0].marcado = (r.nota < ultimo.nota);

    reconstroiHeap(h, 0, n, est);
}

void reconstroiHeap(Heap *h, int i, int n, Estatisticas* est) {
    int menorIndice = i;
    int esq = 2 * i + 1;
    int dir = 2 * i + 2;

    if (esq < n && menor(h->v[esq], h->v[menorIndice], est))
        menorIndice = esq;

    if (dir < n && menor(h->v[dir], h->v[menorIndice], est))
        menorIndice = dir;

    if (menorIndice != i) {
        ItemHeap aux = h->v[i];
        h->v[i] = h->v[menorIndice];
        h->v[menorIndice] = aux;

        reconstroiHeap(h, menorIndice, n, est);
    }
}
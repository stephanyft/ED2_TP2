#include "subSelecao.h"

//Funçao que verifica qual item eh menor
int menor(ItemHeap a, ItemHeap b, Estatisticas* est) {
    // marcas diferentes: o nao marcado eh menor
    if (a.marcado != b.marcado) {
        est->comparacoes++;
        return a.marcado < b.marcado;
    }
    
    // mesma marca: compara as notas
    est->comparacoes++;
    return a.reg.nota < b.reg.nota;
}

//Funçao que verifica se todos os itens do heap estao marcados
bool fimBloco(Heap *h, int n) {
    for (int i = 0; i < n; i++) {
        if (!h->v[i].marcado)
            return false;
    }

    return true;
}

//Funçao que desmarca todos os itens do heap, caso todos estejam marcados
void preparaHeap(Heap *h, int n, Estatisticas* est) {
    if (fimBloco(h, n)) {
        novoBloco(h, n, est);
    }
}

//Funçao que desmarca todos os itens do bloco e o reorganiza
void novoBloco(Heap *h, int n, Estatisticas* est) {
    for (int i = 0; i < n; i++) {
        h->v[i].marcado = false;
    }

    constroiHeap(h, n, est);
}

//Funçao que transforma o vetor inicial em um heap valido
void constroiHeap(Heap *h, int n, Estatisticas* est) {
    for (int i = n / 2 - 1; i >= 0; i--) {
        reconstroiHeap(h, i, n, est);
    }
}

//Substitui a raiz do heap pelo novo registro, comparando as notas para marcar se nescessario
void substituiRaiz(Heap *h, Registro r, Registro ultimo, int n, Estatisticas* est) {
    h->v[0].reg = r;
    h->v[0].marcado = comparaNota(est, &r, &ultimo) < 0 ? true : false;

    reconstroiHeap(h, 0, n, est);
}

//Funçao que reconstroi o vetor em um heap valido
void reconstroiHeap(Heap *h, int i, int n, Estatisticas* est) {
    int menorIndice = i;
    int esq = 2 * i + 1;
    int dir = 2 * i + 2;

    //Verifica se o filho da esquerda existe e eh menor que o pai
    if (esq < n && menor(h->v[esq], h->v[menorIndice], est))
        menorIndice = esq;

    //verifica se o filho da direita existe e eh menor que o menor encontrado ate aqui
    if (dir < n && menor(h->v[dir], h->v[menorIndice], est))
        menorIndice = dir;

    // se algum filho era menor, troca e continua descendo
    if (menorIndice != i) {
        ItemHeap aux = h->v[i];
        h->v[i] = h->v[menorIndice];
        h->v[menorIndice] = aux;

        reconstroiHeap(h, menorIndice, n, est);
    }
}
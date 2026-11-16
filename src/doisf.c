#include "../includes/doisf.h"
#include "subSelecao.h"

int metodoDoisFCaminhos(FILE *arq, int n, Estatisticas *est) {
    if(arq == NULL)
        return 0;
    
    Fita* f[20];

    for (int i = 0; i < 20; i++) {
        f[i] = malloc(sizeof(Fita));

        if (f[i] == NULL)
            return 0;

        fitaAbre(f[i], i + 1);
    }

    ordenaBlocosIniciais(arq, n, f, est);
    intercalaFitas(f);
    
    return 1;
}

int ordenaBlocosIniciais(FILE *arq, int n, Fita* f[], Estatisticas *est) {
    Heap h;
    Registro reg;

    fseek(arq, 0, SEEK_SET);

    int lidos = 0;
    int cont = 0;
    int qtdHeap = 0;
    int fitaAtual = 0;

    while (qtdHeap < 10 && lidos < n && registroLeTexto(arq, &reg, est) == 1) {

        h.v[qtdHeap].reg = reg;
        h.v[qtdHeap].marcado = false;

        qtdHeap++;
        lidos++;
    }

    constroiHeap(&h, qtdHeap, est);

    while (qtdHeap > 0) {
        fitaIniciaBloco(f[fitaAtual]);
        escreveBloco(arq, f[fitaAtual], &h, &cont, &lidos, n, &qtdHeap, est);
        fitaFinalizaBloco(f[fitaAtual]);
        fitaAtual = (fitaAtual + 1) % 10;
    }

    return cont == n;
}

void escreveBloco(FILE *arq, Fita *f, Heap *h, int *cont, int *lidos, int n, int *qtdHeap, Estatisticas *est) {
    Registro novo;
    Registro ultimo;

    preparaHeap(h, *qtdHeap, est);

    while (*qtdHeap > 0) {
        if (fimBloco(h, *qtdHeap))
            break;

        h->v[0].fitaOrigem = f->indice;
        ultimo = h->v[0].reg;        

        fitaEscreve(f, &ultimo, est);
        (*cont)++;

        if ((*lidos) < n && registroLeTexto(arq, &novo, est) == 1) {
            (*lidos)++;

            h->v[0].reg = novo;
            h->v[0].marcado = comparaNota(est, &novo, &ultimo) < 0 ? true : false;
        } 
        else {
            h->v[0] = h->v[*qtdHeap - 1];
            (*qtdHeap)--;
        }

        if (*qtdHeap > 0)
            reconstroiHeap(h, 0, *qtdHeap, est);
    }
}

int intercalaFitas(Fita* f[]) {
    return 1;   
}
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

    for(int i = 0; i < 20; i++) {
        for(int j = 0; j < f[i]->numBlocos; j++) {
            printf("Quantidade de registros na fita %d: %ld\n", i, f[i]->tamBlocos[j]);
        }
        printf("Quantidade de blocos na fita %d: %ld\n", i, f[i]->numBlocos);
    }
    intercalaFitas(f, est, n);
    
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

bool verificaSeTemItens(Fita* f[]) {
    for(int i = 0; i < 20; i++) {
        if(!fitaTemBloco(f[i])) {
            return false;
        }
    }
    return true;
}

int intercalaFitas(Fita* f[], Estatisticas* est, int n) {
    if(f == NULL) {
        printf("erro na função intercalaFitas\n");
        return -1;
    }

    Heap h;
    Registro r;

    for(int i = 0; i < 10; i++) {
        fitaRebobina(f[i]);
    }

    for(int j = 10; j < 20; j++) {
        fitaReinicia(f[j]);
    }

    int fitaSaidaAtual = 0;
    int qtdHeap = 0;
    int itensLidos = 0;

    printf("Linha 117\n");
    for(int k = 0; k < 10; k++) {
        printf("Linha 119\n");

        if(fitaTemBloco(f[k]) == 1) {
            printf("Linha 122\n");

            if(fitaLe(f[k], &r, est) == 1) {
                qtdHeap++;
                h.v[k].reg = r;
                h.v[k].fitaOrigem = k;
                h.v[k].marcado = false;
                itensLidos++;
            } 
        }
    }

    constroiHeap(&h, TAM_HEAP, est);
    printf("Linha 135\n");

    while(verificaSeTemItens(f)) {
        printf("Linha 139\n");
        fitaSaidaAtual = (fitaSaidaAtual + 1) % 10 + 10;
        fitaIniciaBloco(f[fitaSaidaAtual - 1]);
        
        while(qtdHeap > 0) {
            printf("Linha 144\n");
            fitaEscreve(f[fitaSaidaAtual-1], &(h.v[0].reg), est);
            int resultLeitura = fitaLe(f[h.v[0].fitaOrigem], &r, est);
        
            if(!resultLeitura) {
                printf("Linha 149\n");
                h.v[0] = h.v[qtdHeap];
                qtdHeap--;
                reconstroiHeap(&h, 0, qtdHeap, est);
                continue;
            }
        
            substituiRaiz(&h, r, h.v[0].reg, qtdHeap, est);
            itensLidos++;
        }

        fitaFinalizaBloco(f[fitaSaidaAtual - 1]);
        fitaSaidaAtual++;

    }
}
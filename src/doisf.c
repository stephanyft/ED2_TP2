#include "../includes/doisf.h"
#include "subSelecao.h"

int metodoDoisFCaminhos(FILE *arq, int n, int imprimir, Estatisticas *est) {
    if(arq == NULL)
        return 0;
    
    Fita* f[20];

    for (int i = 0; i < 20; i++) {
        f[i] = malloc(sizeof(Fita));

        if (f[i] == NULL)
            return 0;

        fitaAbre(f[i], i + 1);
    }

    if (imprimir) {
        Registro tmp;
        printf("\n===== Dados antes de ordenar =====\n\n");
        fseek(arq, 0, SEEK_SET);
        for (int i = 0; i < n && registroLeTexto(arq, &tmp, NULL); i++)
            registroImprime(&tmp);
        printf("\n==================================\n");
        fseek(arq, 0, SEEK_SET);
    }
    
    estatisticasIniciaTempo(est);
    ordenaBlocosIniciais(arq, n, f, est);

    int num = intercalaFitas(f, est);
    estatisticasParaTempo(est);
    
    if (num < 0) {
        return 0;
    }

    if(imprimir) {
        printf("\n===== Dados depois de ordenar =====\n\n");
        fitaImprime(f[num]);
        printf("\n==================================\n");
    }
    
    Registro r;
    FILE *arq2 = fopen("ordenado", "wb");
    if(arq2 == NULL) {
        printf("\nFalha ao criar arquivo ordenado!\n");
    }
    else {
        fitaRebobina(f[num]);
        while (fitaLe(f[num], &r, NULL)) {
            registroEscreveTexto(arq2, &r, est);
        }
        fclose(arq2);
    }

    for (int i = 0; i < 20; i++) {
        fitaFecha(f[i]);
        free(f[i]);
    }

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

bool verificaSeTemBloco(Fita* f[], int inicioEntrada) {
    for(int i = inicioEntrada; i < (inicioEntrada + 10); i++) {
        if(fitaTemBloco(f[i])) {
            return true;
        }
    }
    return false;
}

bool ultimoBloco(Fita* f[], int inicioSaida) {
    int cont = 0;
    for(int i = inicioSaida; i < (inicioSaida + 10); i++) {
        if(fitaTemBloco(f[i])) {
            cont++;
        }
    }
    if(cont != 1)
        return false;
    return true;
}

int intercalaFitas(Fita* f[], Estatisticas* est) {
    if(f == NULL) {
        printf("erro na função intercalaFitas\n");
        return -1;
    }

    Heap h;
    Registro r;
    int inicioEntrada = 0, inicioSaida = 10;

    for(int i = 0; i < 10; i++) {
        fitaRebobina(f[i]);
    }

    for(int j = 10; j < 20; j++) {
        fitaReinicia(f[j]);
    }
    
    int fitaSaidaAtual = -1;
    int qtdHeap = 0;
    int itensLidos = 0;
    while(1) {
        while(verificaSeTemBloco(f, inicioEntrada)) {
            for(int k = inicioEntrada; k < (inicioEntrada + 10); k++) {
                if(fitaTemBloco(f[k]) == 1) {
                    if(fitaLe(f[k], &r, est) == 1) {
                        h.v[qtdHeap].reg = r;
                        h.v[qtdHeap].fitaOrigem = k;
                        h.v[qtdHeap].marcado = false;
                        qtdHeap++;
                        itensLidos++;
                    } 
                }
            }
            constroiHeap(&h, qtdHeap, est);
    
            fitaSaidaAtual = (fitaSaidaAtual + 1) % 10;
            if(inicioSaida == 10)
                fitaSaidaAtual += 10;
            fitaIniciaBloco(f[fitaSaidaAtual]);
            
            while(qtdHeap > 0) {
                fitaEscreve(f[fitaSaidaAtual], &(h.v[0].reg), est);
                int resultLeitura = fitaLe(f[h.v[0].fitaOrigem], &r, est);
            
                if(!resultLeitura) {
                    h.v[0] = h.v[qtdHeap-1];
                    qtdHeap--;
                    reconstroiHeap(&h, 0, qtdHeap, est);
                    continue;
                }
                
                substituiRaiz(&h, r, h.v[0].reg, qtdHeap, est);
                itensLidos++;
            }
            
            fitaFinalizaBloco(f[fitaSaidaAtual]);
            for(int l = inicioEntrada; l < (inicioEntrada + 10); l++) {
                fitaProximoBloco(f[l]);
            }
            
        }

        if (ultimoBloco(f, inicioSaida)) {
            for (int i = inicioSaida; i < inicioSaida + 10; i++)
                if (fitaTemBloco(f[i])) { 
                    fitaRebobina(f[i]); 
                    return i;
                }
        }

        for(int i = inicioSaida; i < (inicioSaida + 10); i++) {
            fitaRebobina(f[i]);
        }
        for (int i = inicioEntrada; i < inicioEntrada + 10; i++) {
            fitaReinicia(f[i]);
        }

        int temp = inicioEntrada;
        inicioEntrada = inicioSaida;
        inicioSaida = temp;
    }
}
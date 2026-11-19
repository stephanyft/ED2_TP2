#include <stdio.h>
#include <stdlib.h>
#include "../includes/metodof+1.h"

void gerarBlocosFMaisUm(FILE *arq, Fita *fitas, int quantidade, Estatisticas *est) {
    Heap h;
    int tamHeap = 0;
    int lidosTotal = 0;
    Registro r;

    /* 1. Preenche a memoria inicial (ate 19 registros) */
    while (tamHeap < MEMORIA_F1 && lidosTotal < quantidade && registroLeTexto(arq, &r, est)) {
        h.v[tamHeap].reg = r;
        h.v[tamHeap].marcado = false;
        h.v[tamHeap].fitaOrigem = -1;
        tamHeap++;
        lidosTotal++;
    }

    if (tamHeap == 0) {
        return;
    }

    constroiHeap(&h, tamHeap, est);

    int fitaAtual = 0;
    fitaIniciaBloco(&fitas[fitaAtual]);

    /* 2. Processa os registros mantendo o heap de substituicao */
    while (tamHeap > 0) {
        /* Se todos os elementos no heap estao marcados, encerra o bloco atual e inicia um novo na proxima fita */
        if (fimBloco(&h, tamHeap)) {
            fitaFinalizaBloco(&fitas[fitaAtual]);
            fitaAtual = (fitaAtual + 1) % F_ENTRADA_F1;
            fitaIniciaBloco(&fitas[fitaAtual]);
            novoBloco(&h, tamHeap, est);
        }

        /* Retira o menor registro nao marcado (raiz do heap) e grava na fita */
        Registro ultimo = h.v[0].reg;
        fitaEscreve(&fitas[fitaAtual], &ultimo, est);

        /* Tenta ler o proximo registro do arquivo de entrada para substituir a raiz */
        if (lidosTotal < quantidade && registroLeTexto(arq, &r, est)) {
            lidosTotal++;
            substituiRaiz(&h, r, ultimo, tamHeap, est);
        } else {
            /* Arquivo de entrada acabou: reduz o heap movendo o ultimo elemento para a raiz */
            tamHeap--;
            if (tamHeap > 0) {
                h.v[0] = h.v[tamHeap];
                reconstroiHeap(&h, 0, tamHeap, est);
            }
        }
    }

    fitaFinalizaBloco(&fitas[fitaAtual]);
}


void redistribuirFMaisUm(Fita *fitaSaida, Fita *fitasEntrada, Estatisticas *est) {
    for (int i = 0; i < F_ENTRADA_F1; i++) {
        fitaReinicia(&fitasEntrada[i]);
    }

    fitaRebobina(fitaSaida);
    int fitaDest = 0;
    Registro r;

    while (fitaTemBloco(fitaSaida)) {
        fitaIniciaBloco(&fitasEntrada[fitaDest]);
        while (fitaLe(fitaSaida, &r, est)) {
            fitaEscreve(&fitasEntrada[fitaDest], &r, est);
        }
        fitaFinalizaBloco(&fitasEntrada[fitaDest]);

        fitaProximoBloco(fitaSaida);
        fitaDest = (fitaDest + 1) % F_ENTRADA_F1;
    }

    fitaReinicia(fitaSaida);
}


int intercalarFMaisUm(Fita *fitas, Estatisticas *est) {
    int fitaSaidaIdx = F_ENTRADA_F1; /* Fita 19 eh a fita de saida */

    while (1) {
        long totalBlocos = 0;
        int ultimaFitaComBloco = 0;

        for (int i = 0; i < F_ENTRADA_F1; i++) {
            long nb = fitaNumBlocos(&fitas[i]);
            totalBlocos += nb;
            if (nb > 0) {
                ultimaFitaComBloco = i;
            }
        }

        /* Condicao de parada: resta no maximo 1 bloco ordenado em todas as fitas */
        if (totalBlocos <= 1) {
            return ultimaFitaComBloco;
        }

        /* Rebobina todas as fitas de entrada e limpa a fita de saida */
        for (int i = 0; i < F_ENTRADA_F1; i++) {
            fitaRebobina(&fitas[i]);
        }
        fitaReinicia(&fitas[fitaSaidaIdx]);

        /* Enquanto houver blocos ativos em pelo menos uma fita de entrada, faz um merge */
        while (1) {
            Heap h;
            int tamHeap = 0;

            /* Pega o primeiro registro do bloco atual de cada fita de entrada ativa */
            for (int i = 0; i < F_ENTRADA_F1; i++) {
                if (fitaTemBloco(&fitas[i])) {
                    Registro r;
                    if (fitaLe(&fitas[i], &r, est)) {
                        h.v[tamHeap].reg = r;
                        h.v[tamHeap].marcado = false;
                        h.v[tamHeap].fitaOrigem = i;
                        tamHeap++;
                    }
                }
            }

            /* Se nenhuma fita tinha bloco para ler, encerrou esta passada de intercalacao */
            if (tamHeap == 0) {
                break;
            }

            constroiHeap(&h, tamHeap, est);
            fitaIniciaBloco(&fitas[fitaSaidaIdx]);

            /* Intercala os blocos atuais usando o Heap */
            while (tamHeap > 0) {
                ItemHeap menorItem = h.v[0];
                fitaEscreve(&fitas[fitaSaidaIdx], &menorItem.reg, est);

                int origem = menorItem.fitaOrigem;
                Registro prox;

                /* Le o proximo registro do MESMO bloco da fita de origem */
                if (fitaLe(&fitas[origem], &prox, est)) {
                    h.v[0].reg = prox;
                    h.v[0].marcado = false;
                    h.v[0].fitaOrigem = origem;
                    reconstroiHeap(&h, 0, tamHeap, est);
                } else {
                    /* Bloco daquela fita acabou: avanca cursor da fita para o proximo bloco e reduz o heap */
                    fitaProximoBloco(&fitas[origem]);
                    tamHeap--;
                    if (tamHeap > 0) {
                        h.v[0] = h.v[tamHeap];
                        reconstroiHeap(&h, 0, tamHeap, est);
                    }
                }
            }

            fitaFinalizaBloco(&fitas[fitaSaidaIdx]);
        }

        /*
         * Se a fita de saida (19) ficou com apenas 1 bloco, o arquivo ja esta 100% ordenado nela!
         * Evitamos uma redistribuicao desnecessaria.
         */
        if (fitaNumBlocos(&fitas[fitaSaidaIdx]) <= 1) {
            return fitaSaidaIdx;
        }

        /* Caso contrario, redistribui os blocos da fita 19 de volta para as fitas 0..18 */
        redistribuirFMaisUm(&fitas[fitaSaidaIdx], fitas, est);
    }
}

void metodoFMaisUmCaminhos(FILE *arq, int quantidade, Estatisticas *est) {
    if (arq == NULL) {
        fprintf(stderr, "Erro: arquivo de entrada nulo.\n");
        return;
    }

    Fita fitas[NUM_FITAS_F1];
    for (int i = 0; i < NUM_FITAS_F1; i++) {
        fitaAbre(&fitas[i], i + 1);
    }

    estatisticasIniciaTempoPre(est);

    /* 1. Geracao dos blocos ordenados iniciais */
    gerarBlocosFMaisUm(arq, fitas, quantidade, est);

    estatisticasParaTempoPre(est);

    estatisticasIniciaTempoOrd(est);

    /* 2. Intercalacao balanceada f+1 caminhos */
    int fitaFinal = intercalarFMaisUm(fitas, est);

    estatisticasParaTempoOrd(est);

    /* Descomente abaixo caso queira gravar em um arquivo texto de saida ou imprimir */
    /* fitaImprime(&fitas[fitaFinal]); */
    (void)fitaFinal;

    for (int i = 0; i < NUM_FITAS_F1; i++) {
        fitaFecha(&fitas[i]);
    }
}
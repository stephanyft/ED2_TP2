#include <stdio.h>
#include <stdlib.h>
#include "../includes/metodof+1.h"

// Gera os blocos iniciais ordenados via selecao por substituicao
void gerarBlocosFMaisUm(FILE *arq, Fita *fitas, int quantidade, Estatisticas *est) {
    Heap h;
    int tamHeap = 0;
    int lidosTotal = 0;
    Registro r;

    // Carrega os primeiros registros na memoria (maximo 19)
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

    while (tamHeap > 0) {
        // Todos marcados -> fecha o bloco atual e passa para a proxima fita
        if (fimBloco(&h, tamHeap)) {
            fitaFinalizaBloco(&fitas[fitaAtual]);
            fitaAtual = (fitaAtual + 1) % F_ENTRADA_F1;
            fitaIniciaBloco(&fitas[fitaAtual]);
            novoBloco(&h, tamHeap, est);
        }

        Registro ultimo = h.v[0].reg;
        fitaEscreve(&fitas[fitaAtual], &ultimo, est);

        // Le o proximo registro para substituir a raiz ou reduz o heap no fim da entrada
        if (lidosTotal < quantidade && registroLeTexto(arq, &r, est)) {
            lidosTotal++;
            substituiRaiz(&h, r, ultimo, tamHeap, est);
        } else {
            tamHeap--;
            if (tamHeap > 0) {
                h.v[0] = h.v[tamHeap];
                reconstroiHeap(&h, 0, tamHeap, est);
            }
        }
    }

    fitaFinalizaBloco(&fitas[fitaAtual]);
}

// Copia os blocos da fita de saida de volta para as 19 fitas de entrada
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

// Intercala as 19 fitas na fita de saida ate sobrar 1 bloco; retorna o indice da fita final
int intercalarFMaisUm(Fita *fitas, Estatisticas *est) {
    int fitaSaidaIdx = F_ENTRADA_F1;

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

        if (totalBlocos <= 1) {
            return ultimaFitaComBloco;
        }

        for (int i = 0; i < F_ENTRADA_F1; i++) {
            fitaRebobina(&fitas[i]);
        }
        fitaReinicia(&fitas[fitaSaidaIdx]);

        // Intercala uma leva de blocos por vez ate esgotar todas as fitas de entrada
        while (1) {
            Heap h;
            int tamHeap = 0;

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

            if (tamHeap == 0) {
                break;
            }

            constroiHeap(&h, tamHeap, est);
            fitaIniciaBloco(&fitas[fitaSaidaIdx]);

            while (tamHeap > 0) {
                ItemHeap menorItem = h.v[0];
                fitaEscreve(&fitas[fitaSaidaIdx], &menorItem.reg, est);

                int origem = menorItem.fitaOrigem;
                Registro prox;

                // Tenta puxar o proximo item do mesmo bloco; se acabou, remove a fita do heap
                if (fitaLe(&fitas[origem], &prox, est)) {
                    h.v[0].reg = prox;
                    h.v[0].marcado = false;
                    h.v[0].fitaOrigem = origem;
                    reconstroiHeap(&h, 0, tamHeap, est);
                } else {
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

        // Se sobrou so 1 bloco na fita de saida, ja terminou e nao precisa redistribuir
        if (fitaNumBlocos(&fitas[fitaSaidaIdx]) <= 1) {
            return fitaSaidaIdx;
        }

        redistribuirFMaisUm(&fitas[fitaSaidaIdx], fitas, est);
    }
}

void metodoFMaisUmCaminhos(FILE *arq, int quantidade, int imprimir, Estatisticas *est) {
    if (arq == NULL) {
        fprintf(stderr, "Erro: arquivo de entrada nulo.\n");
        return;
    }

    Fita fitas[NUM_FITAS_F1];
    for (int i = 0; i < NUM_FITAS_F1; i++) {
        fitaAbre(&fitas[i], i + 1);
    }

    if (imprimir) {
        Registro tmp;
        printf("\n===== Dados antes de ordenar =====\n\n");
        fseek(arq, 0, SEEK_SET);
        for (int i = 0; i < quantidade && registroLeTexto(arq, &tmp, NULL); i++)
            registroImprime(&tmp);
        printf("\n==================================\n");
        fseek(arq, 0, SEEK_SET);
    }

    // Fase 1: geracao dos blocos
    estatisticasIniciaTempoPre(est);
    gerarBlocosFMaisUm(arq, fitas, quantidade, est);
    estatisticasParaTempoPre(est);

    // Fase 2: intercalacao
    estatisticasIniciaTempoOrd(est);
    int fitaFinal = intercalarFMaisUm(fitas, est);
    estatisticasParaTempoOrd(est);

    if (imprimir) {
        printf("\n===== Dados depois de ordenar =====\n\n");
        fitaImprime(&fitas[fitaFinal]);
        printf("\n==================================\n");
    }

    // Grava o resultado final em texto sem contabilizar nas estatisticas
    Registro r;
    FILE *arq2 = fopen("ordenado_f+1.txt", "wb");
    if (arq2 == NULL) {
        printf("\nFalha ao criar arquivo ordenado!\n");
    } else {
        fitaRebobina(&fitas[fitaFinal]);
        while (fitaLe(&fitas[fitaFinal], &r, NULL)) {
            registroEscreveTexto(arq2, &r, NULL);
        }
        fclose(arq2);
    }

    (void)fitaFinal;

    for (int i = 0; i < NUM_FITAS_F1; i++) {
        fitaFecha(&fitas[i]);
    }
}
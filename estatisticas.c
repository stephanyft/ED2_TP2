#include <stdio.h>
#include "estatisticas.h"

void estatisticasInicia(Estatisticas *est) {
    est->leituras = 0;
    est->escritas = 0;
    est->comparacoes = 0;
    est->inicio.tv_sec = est->inicio.tv_nsec = 0;
    est->fim.tv_sec = est->fim.tv_nsec = 0;
}

void estatisticasIniciaTempo(Estatisticas *est) {
    clock_gettime(CLOCK_MONOTONIC, &est->inicio);
}

void estatisticasParaTempo(Estatisticas *est) {
    clock_gettime(CLOCK_MONOTONIC, &est->fim);
}

double estatisticasTempo(const Estatisticas *est) {
    return (double)(est->fim.tv_sec - est->inicio.tv_sec)
         + (double)(est->fim.tv_nsec - est->inicio.tv_nsec) / 1e9;
}

void estatisticasImprime(const Estatisticas *est) {
    printf("\n===== Estatisticas de execucao =====\n");
    printf("Leituras (externa -> interna) : %ld\n", est->leituras);
    printf("Escritas (interna -> externa) : %ld\n", est->escritas);
    printf("Comparacoes entre chaves      : %ld\n", est->comparacoes);
    printf("Tempo de execucao             : %.6f s\n", estatisticasTempo(est));
}

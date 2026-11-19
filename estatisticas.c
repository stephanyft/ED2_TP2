#include <stdio.h>
#include "estatisticas.h"

void estatisticasInicia(Estatisticas *est) {
    est->leituras = 0;
    est->escritas = 0;
    est->comparacoes = 0;
    est->inicioPre.tv_sec = est->inicioPre.tv_nsec = 0;
    est->fimPre.tv_sec = est->fimPre.tv_nsec = 0;
    est->inicioOrd.tv_sec = est->inicioOrd.tv_nsec = 0;
    est->fimOrd.tv_sec = est->fimOrd.tv_nsec = 0;
}

void estatisticasIniciaTempoPre(Estatisticas *est) {
    clock_gettime(CLOCK_MONOTONIC, &est->inicioPre);
}

void estatisticasIniciaTempoOrd(Estatisticas *est) {
    clock_gettime(CLOCK_MONOTONIC, &est->inicioOrd);
}

void estatisticasParaTempoPre(Estatisticas *est) {
    clock_gettime(CLOCK_MONOTONIC, &est->fimPre);
}

void estatisticasParaTempoOrd(Estatisticas *est) {
    clock_gettime(CLOCK_MONOTONIC, &est->fimOrd);
}

double estatisticasTempoPre(const Estatisticas *est) {
    return (double)(est->fimPre.tv_sec - est->inicioPre.tv_sec)
         + (double)(est->fimPre.tv_nsec - est->inicioPre.tv_nsec) / 1e9;
}

double estatisticasTempoOrd(const Estatisticas *est) {
    return (double)(est->fimOrd.tv_sec - est->inicioOrd.tv_sec)
         + (double)(est->fimOrd.tv_nsec - est->inicioOrd.tv_nsec) / 1e9;
}

void estatisticasImprime(const Estatisticas *est) {
    printf("\n===== Estatisticas de execucao =====\n");
    printf("Leituras (externa -> interna) : %ld\n", est->leituras);
    printf("Escritas (interna -> externa) : %ld\n", est->escritas);
    printf("Comparacoes entre chaves      : %ld\n", est->comparacoes);
    printf("Tempo de pre-processamento    : %.6f s\n", estatisticasTempoPre(est));
    printf("Tempo de ordenaçao            : %.6f s\n", estatisticasTempoOrd(est));
}

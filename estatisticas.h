#ifndef ESTATISTICAS_H
#define ESTATISTICAS_H

#include <time.h>

/*
 * Quesitos de analise exigidos pelo trabalho. Uma unica instancia e criada
 * no main e passada por ponteiro para todas as funcoes que fazem I/O de
 * registros ou comparam chaves.
 *
 * Regra: nenhum fread/fwrite/fgets de registro e nenhuma comparacao de
 * nota deve acontecer fora de registro.c e fita.c, para que a contagem
 * nunca fique inconsistente.
 */
typedef struct {
    long leituras;       /* registros transferidos da memoria externa para a interna */
    long escritas;       /* registros transferidos da memoria interna para a externa */
    long comparacoes;    /* comparacoes entre chaves (notas) */
    struct timespec inicioPre;
    struct timespec fimPre;
    struct timespec inicioOrd;
    struct timespec fimOrd;
} Estatisticas;

/* Zera os contadores. */
void estatisticasInicia(Estatisticas *est);

/* Marca o instante de inicio / fim da execucao. */
void estatisticasIniciaTempoPre(Estatisticas *est);
void estatisticasParaTempoPre(Estatisticas *est);

void estatisticasIniciaTempoOrd(Estatisticas *est);
void estatisticasParaTempoOrd(Estatisticas *est);

/* Tempo decorrido entre inicio e fim, em segundos. */
double estatisticasTempo(const Estatisticas *est);

/* Imprime os quatro quesitos na tela. */
void estatisticasImprime(const Estatisticas *est);

#endif

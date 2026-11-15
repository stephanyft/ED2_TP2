#ifndef FITA_H
#define FITA_H

#include <stdio.h>
#include "registro.h"
#include "estatisticas.h"

/*
 * Uma fita eh um arquivo binario temporario com registros de tamanho fixo,
 * organizado em BLOCOS (corridas ordenadas). A fita guarda o tamanho de
 * cada bloco em um vetor, de modo que o leitor sempre sabe onde um bloco
 * termina e o proximo comeca, sem depender de comparacao de chaves.
 *
 * Ciclo de vida tipico:
 *
 *   fitaAbre(&f, 1);
 *   // escrita
 *   fitaIniciaBloco(&f);  fitaEscreve(&f, &r, est); ...  fitaFinalizaBloco(&f);
 *   fitaIniciaBloco(&f);  ...                            fitaFinalizaBloco(&f);
 *   // leitura
 *   fitaRebobina(&f);
 *   do {
 *       while (fitaLe(&f, &r, est)) { ... }   // registros do bloco atual
 *   } while (fitaProximoBloco(&f));
 *   // reutilizar na proxima etapa
 *   fitaReinicia(&f);
 *   ...
 *   fitaFecha(&f);
 *
 * Toda fitaEscreve conta uma escrita e toda fitaLe conta uma leitura
 * em est (se est != NULL).
 */
typedef struct {
    FILE *arq;
    char nome[32];       /* nome do arquivo temporario em disco */
    int indice;

    /* blocos gravados */
    long numBlocos;
    long *tamBlocos;     /* tamBlocos[i] = quantidade de registros do bloco i */
    long capBlocos;      /* capacidade alocada de tamBlocos */
    int blocoAberto;     /* 1 se ha um bloco em construcao */

    /* cursor de leitura */
    long blocoAtual;     /* indice do bloco sendo lido */
    long lidosNoBloco;   /* quantos registros do bloco atual ja foram lidos */
} Fita;

/* Cria (ou trunca) o arquivo temporario da fita de numero indice. */
void fitaAbre(Fita *f, int indice);

/* Fecha e apaga o arquivo temporario; libera memoria. */
void fitaFecha(Fita *f);

/* Esvazia a fita (trunca o arquivo e zera os blocos) para reutilizacao. */
void fitaReinicia(Fita *f);

/* ---- escrita ---- */

/* Comeca um novo bloco. A fita deve estar posicionada no fim (apos reinicio ou escrita). */
void fitaIniciaBloco(Fita *f);

/* Grava um registro no bloco aberto. Conta uma escrita. */
void fitaEscreve(Fita *f, const Registro *r, Estatisticas *est);

/* Encerra o bloco aberto, registrando seu tamanho. Blocos vazios sao descartados. */
void fitaFinalizaBloco(Fita *f);

/* ---- leitura ---- */

/* Posiciona a leitura no inicio do bloco 0. */
void fitaRebobina(Fita *f);

/*
 * Le o proximo registro do BLOCO ATUAL. Conta uma leitura.
 * Retorna 0 (sem ler nada) quando o bloco atual acabou ou nao ha blocos.
 */
int fitaLe(Fita *f, Registro *r, Estatisticas *est);

/* Avanca para o proximo bloco. Retorna 0 se nao ha mais blocos. */
int fitaProximoBloco(Fita *f);

/* 1 se o cursor esta sobre um bloco valido (ainda ha o que ler nesta fita). */
int fitaTemBloco(const Fita *f);

/* Quantidade de blocos gravados. */
long fitaNumBlocos(const Fita *f);

/* Quantidade de registros do bloco i. */
long fitaTamBloco(const Fita *f, long i);

/* Total de registros gravados na fita. */
long fitaTotalRegistros(const Fita *f);

/* Imprime todos os registros da fita na tela, bloco a bloco. Nao conta I/O. */
void fitaImprime(Fita *f);

#endif

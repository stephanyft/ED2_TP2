#ifndef REGISTRO_H
#define REGISTRO_H

#include <stdio.h>
#include "estatisticas.h"

/*
 * Um registro corresponde a uma linha do arquivo PROVAO.TXT.
 * Cada linha tem exatamente 99 caracteres uteis seguidos de "\r\n":
 *   colunas  1 a  8 : numero de inscricao (inteiro longo)
 *   colunas 10 a 14 : nota (real entre 0.0 e 100.0, formato "034.8")
 *   colunas 16 a 17 : estado (2 caracteres)
 *   colunas 19 a 68 : cidade (50 caracteres)
 *   colunas 70 a 99 : curso  (30 caracteres)
 */
#define TAM_LINHA_TEXTO 99
#define TAM_BUFFER_LINHA 128

typedef struct {
    long inscricao;
    float nota;          /* campo de ordenacao */
    char estado[3];
    char cidade[51];
    char curso[31];
} Registro;

/*
 * Converte uma linha de texto (no formato do PROVAO.TXT) em registro.
 * Retorna 1 em sucesso, 0 se a linha for curta demais ou mal formada.
 * Nao conta leitura: quem leu a linha eh que contabiliza.
 */
int registroDeLinha(const char *linha, Registro *r);

/*
 * Le a proxima linha do arquivo texto e a converte em registro.
 * Conta UMA leitura em est (se est != NULL).
 * Retorna 1 em sucesso, 0 no fim do arquivo ou em erro.
 */
int registroLeTexto(FILE *arq, Registro *r, Estatisticas *est);

/*
 * Escreve o registro no arquivo texto, no mesmo formato do PROVAO.TXT
 * (99 caracteres + "\r\n"). Conta UMA escrita em est (se est != NULL).
 */
void registroEscreveTexto(FILE *arq, const Registro *r, Estatisticas *est);

/* Imprime o registro na tela, em uma linha. Nao conta nada. */
void registroImprime(const Registro *r);

/*
 * Compara as notas de dois registros e conta UMA comparacao em est.
 * Retorna negativo se a < b, zero se iguais, positivo se a > b.
 * TODA comparacao entre chaves nos algoritmos deve passar por aqui.
 */
int comparaNota(Estatisticas *est, const Registro *a, const Registro *b);

#endif

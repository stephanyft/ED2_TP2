#ifndef INTERCALACAO_F_MAIS_1_H
#define INTERCALACAO_F_MAIS_1_H

#include <stdio.h>
#include "registro.h"
#include "estatisticas.h"
#include "fita.h"

#define F_FITAS 19
#define NUM_FITAS_TOTAL (F_FITAS + 1)
#define TAM_MEMORIA_F1 19

typedef struct {
    Registro reg;
    int marcado;
} NoHeapSubstituiçao;

typedef struct{
    Registro reg;
    int idFita;
} NoHeapIntercalacao;

void IntercalacaoFmais1(FILE *arqEntrada, int numRegistros, Estatisticas *est);

HeapIntercalacao *heap;


#endif

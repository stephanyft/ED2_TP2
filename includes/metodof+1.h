#ifndef INTERCALACAO_F_MAIS_1_H
#define INTERCALACAO_F_MAIS_1_H

#include <stdio.h>
#include "../registro.h"
#include "../estatisticas.h"
#include "../fita.h"
#include "../subSelecao.h"

#define F_ENTRADA_F1 19
#define NUM_FITAS_F1 20
#define MEMORIA_F1 19

// Executa o metodo de intercalacao balanceada f+1 fitas (19 entrada + 1 saida)
void metodoFMaisUmCaminhos(FILE *arq, int quantidade, int imprimir, Estatisticas *est);

void gerarBlocosFMaisUm(FILE *arq, Fita *fitas, int quantidade, Estatisticas *est);
int intercalarFMaisUm(Fita *fitas, Estatisticas *est);
void redistribuirFMaisUm(Fita *fitaSaida, Fita *fitasEntrada, Estatisticas *est);

#endif
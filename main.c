#include<stdio.h>
#include "doisf.h"
#include "subSelecao.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "registro.h"

int main(int argc, char *argv[]) {
    if(argc < 4 || argc > 5){
        printf("Uso: %s <metodo> <quantidade> <situacao> [-P]\n", argv[0]);
        return 1;
    }

    int metodo = atoi(argv[1]);
    int quantidade = atoi(argv[2]);
    int situacao = atoi(argv[3]);
    int imprimir = 0;

    if (argc == 5) {
        if(strcmp(argv[4], "-P") == 0)
            imprimir = 1;
        else{
            printf("Erro: argumento opcional invalido (use -P)\n");
            return 1;
        }
    }

    if(metodo < 1 || metodo > 3) {
        printf("Erro: metodo deve ser numerado de 1 a 3\n");
        return 1;
    }
    if(situacao < 1 || situacao > 3) {
        printf("Erro: situaçao deve ser 1 (asc), (des) ou 3 (aleatorio)\n");
        return 1;
    }

    FILE* arq;
    switch (situacao) {
        case 1:
            arq = fopen("PROVAO_CRESC.TXT", "r");
            break;
        case 2:
            arq = fopen("PROVAO_DECRESC.TXT", "r");
            break;
        case 3:
            arq = fopen("PROVAO.TXT", "r");
            break;
    }

    Estatisticas est;
    estatisticasInicia(&est);

    switch(metodo) {
        case 1:
            metodoDoisFCaminhos(arq, quantidade, imprimir, &est);
            break;
        case 2:
            printf("Metodo 2 ainda nao implemtnado\n");
            break;
        case 3: 
            metodoQuicksortExterno(arq, quantidade, imprimir, &est);
            break;
    }
    return 0;
}
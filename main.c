#include<stdio.h>
#include "doisf.h"
#include "subSelecao.h"

int main() {
    FILE* arq = fopen("PROVAO.TXT", "r");
    Estatisticas est;

    estatisticasInicia(&est);

    int result = metodoDoisFCaminhos(arq, 2000, &est);

    return 0;
}
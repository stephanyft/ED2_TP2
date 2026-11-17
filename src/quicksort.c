#include "../include/quicksort.h"
#include <stdbool.h>
#include <float.h>

// Grava os registros restantes do vetor pivo
int gravaPivo(FILE *arq, Registro pivo[], int N,long EscritaEsq, int *transfEscrita) {

    for (int i = 0; i < N; i++) {

        if (fseek(arq, EscritaEsq * sizeof(Registro), SEEK_SET) != 0) {
            printf("Erro ao posicionar arquivo.\n");
            return 0;
        }

        if (fwrite(&pivo[i], sizeof(Registro), 1, arq) != 1) {
            printf("Erro ao gravar o registro.\n");
            return 0;
        }

        (*transfEscrita)++;
        EscritaEsq++;
    }

    return 1;
}


// Remove o maior registro do vetor pivo
int removeMaiorPivo(FILE *arq, Registro pivo[], int *N, int *transfEscrita, long *EscritaDir,
                    float *Lsup) {

    if (*N <= 0) {
        return 0;
    }

    if (fseek(arq, (*EscritaDir) * sizeof(Registro), SEEK_SET) != 0) {
        printf("Erro ao posicionar o arquivo.\n");
        return 0;
    }

    if (fwrite(&pivo[*N - 1], sizeof(Registro), 1, arq) != 1) {
        printf("Erro ao gravar o registro.\n");
        return 0;
    }

    *Lsup = pivo[*N - 1].nota;

    (*transfEscrita)++;
    (*EscritaDir)--;
    (*N)--;

    return 1;
}


// Particionamento do arquivo
int particiona(FILE *arq, long Esq, long Dir, long *i, long *j,int *transfLeitura, int *transfEscrita, int *comp) {

    long LeituraEsq = Esq;
    long LeituraDir = Dir;

    long EscritaEsq = Esq;
    long EscritaDir = Dir;

    float Linf = -FLT_MAX;
    float Lsup = FLT_MAX;

    bool lerSuperior = true;
    int N = 0;

    // Vetor de memória interna
    Registro pivo[LIMITE_MEMORIA];

    while (LeituraEsq <= LeituraDir) {

        // Definindo qual extremidade sera utilizada
        if (LeituraEsq == EscritaEsq) {
            lerSuperior = false;
        }
        else if (LeituraDir == EscritaDir) {
            lerSuperior = true;
        }

        Registro lido;

        // Leitura do registro
        if (lerSuperior) {

            if (fseek(arq, LeituraDir * sizeof(Registro), SEEK_SET) != 0) {
                printf("Erro ao posicionar o arquivo.\n");
                return 0;
            }

            if (fread(&lido, sizeof(Registro), 1, arq) != 1) {
                printf("Erro ao ler o registro.\n");
                return 0;
            }

            (*transfLeitura)++;
            LeituraDir--;
            lerSuperior = false;
        }
        else {

            if (fseek(arq, LeituraEsq * sizeof(Registro), SEEK_SET) != 0) {
                printf("Erro ao posicionar o arquivo.\n");
                return 0;
            }

            if (fread(&lido, sizeof(Registro), 1, arq) != 1) {
                printf("Erro ao ler o registro.\n");
                return 0;
            }

            (*transfLeitura)++;
            LeituraEsq++;
            lerSuperior = true;
        }

        // Classificação do registro
        (*comp)++;

        if (lido.nota > Lsup) {

            if (fseek(arq, EscritaDir * sizeof(Registro), SEEK_SET) != 0) {
                printf("Erro ao posicionar o arquivo.\n");
                return 0;
            }

            if (fwrite(&lido, sizeof(Registro), 1, arq) != 1) {
                printf("Erro ao gravar o registro.\n");
                return 0;
            }

            (*transfEscrita)++;
            EscritaDir--;
        }
        else {

            (*comp)++;

            if (lido.nota < Linf) {

                if (fseek(arq, EscritaEsq * sizeof(Registro), SEEK_SET) != 0) {
                    printf("Erro ao posicionar o arquivo.\n");
                    return 0;
                }

                if (fwrite(&lido, sizeof(Registro), 1, arq) != 1) {
                    printf("Erro ao gravar o registro.\n");
                    return 0;
                }

                (*transfEscrita)++;
                EscritaEsq++;
            }
            else {
                
            }
        }
    }

    return 1;
}
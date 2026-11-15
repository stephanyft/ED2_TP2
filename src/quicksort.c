#include "../include/quicksort.h"
#include <stdbool.h>
#include <float.h>

int gravaPivo(FILE* arq, Item pivo[], int N, long EscritaEsq, int* transEscrita){ //Laço que percorre o vetor 
    for( int i = 0; i < N; i++){
        if((fseek(arq, EscritaEsq *sizeof(Item), SEEK_SET)) != 0){ //Posiciona o arquivo
            printf("Erro ao posicionar arquivo.\n");
            return 0;
        }
        if((fwrite(&pivo[i], sizeof(Item), 1, arq)) != 1){ //Grava o registro do pivo
            printf("Erro ao gravar o registro.\n");
            return 0; 
        }

        (*transEscrita)++; //atualiza o contador de esrcritas 
        EscritaEsq++; //avança a posição da escrita
    }
    return 1;   
}

int removeMaiorPivo(FILE* arq, Item pivo[], int* N, int *transEscrita, long* EscritaDir, float* Lsup){
    if(*N <= 0){ //Verifica se existem registro disponiveis no pivo.
        return 0 ;
    }
    if((fseek(arq, (*EscritaDir) * sizeof(Item), SEEK_SET)) != 0){ //Posiciona o arquivo 
        printf("Erro ao posicionar o arquivo.\n");
        return 0;
    }
    if((fwrite(&pivo[*N-1], sizeof(Item), 1, arq)) != 1){ //Grava o maior registro
        printf("Erro ao registrar arquivo.\n");
        return 0;
    }

    *Lsup = pivo[*N-1].nota; // Recebe a variavel nota do registro gravado 
    (*transEscrita)++;
    (*EscritaDir)--;
    (*N)--;

    return 1;
}

int particiona(FILE* arq, long Esq, long Dir, int *i, long *j, int* transEscrita, int* transLeitura, int* comp){
    long LeituraEsq = Esq;
    long LeituraDir =  Dir;
    // Variaveis para o contrle de leitura 
    long EscritaEsq = Esq;
    long EscritaDir = Dir;
    //Variaveis para o controle de escrita

    float Linf = -FLT_MAX; //Limite inferior das notas
    float Lsup = FLT_MAX; //Limite superior das notas

    bool lerSuperior = true;
    int N = 0;
    Item pivo[LIMITE_MEMORIA];

    while(LeituraEsq <= LeituraDir){
        // definindo qual extremidade sera feita a leitura
        if(LeituraEsq == EscritaEsq){
            lerSuperior = false;
        }
        else if(LeituraDir == EscritaDir){
            lerSuperior = true;
        }

        Item lido;
        //Leitura
        if(lerSuperior){
            if((fseek(arq, LeituraDir *sizeof(Item), SEEK_SET))!= 0){
                return 0;
            }
            if((fread(&lido, sizeof(Item), 1, arq))!= 1){
                return 0;
            }
            (*transLeitura)++;
            LeituraDir--;
            lerSuperior = false;

        } 
        else{ 
            if((fseek(arq, LeituraEsq *sizeof(Item), SEEK_SET))!= 0){
                return 0;
            }
            if((fread(&lido, sizeof(Item), 1, arq))!= 1){
                return 0;
            }
            (*transLeitura)++;
            LeituraEsq++;
            lerSuperior = true;
        }

        (*comp)++;

        if(lido.nota > Lsup){
           if((fseek(arq, EscritaDir *sizeof(Item), SEEK_SET))!= 0){
            printf("Erro ao posicionar o arquivo.\n");
                return 0;
            }
            if((fwrite(&lido, sizeof(Item), 1, arq)) !=1 ){
                printf("Erro ao gravar o registro.\n");
                return 0;
            }
            (transEscrita)++;
            EscritaDir--;
        }

        else{
            (*comp)++;
            if(lido.nota < Linf){
                if((fseek(arq, EscritaEsq * sizeof(Item), SEEK_SET))!= 0){
                    printf("Erro ao posicionar o arquivo.\n");
                    return 0;
                }
                if((fwrite(&lido, sizeof(Item, 1, arq)))!= 1){
                    printf("Erro ao grava o registro.\n");
                    return 0;
                }
            }
            (*transEscrita)++;
            EscritaEsq++;

        }

        else{
            
        }
    }

    return 1;
}

void inserePivo(FILE* arq, int *transEscrita, Item pivo[], int N, long EscritaEsq ){


}


void quicksortRec(FILE* arq, long Esq, long Dir, int* transEscrita, int* transLeitura , int* comp){


}


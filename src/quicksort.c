#include "../includes/quicksort.h"
#include <stdbool.h>
#include <float.h>

// Lê um registro do arquivo na posição especificada por 'r'
int leRegistro(FILE* arq, long r, Registro* reg, Estatisticas* est){
    if (fseek(arq, r * sizeof(Registro), SEEK_SET) != 0){
        printf("Erro ao posicionar arquivo para leitura.\n");
        return 0;
    }

    if (fread(reg, sizeof(Registro), 1, arq) != 1){
        return 0;
    }
    
    if (est != NULL){
        est->leituras++;
    }
    return 1;
}

// Escreve um registro no arquivo na posição especificada por 'r'
int escreveRegistro(FILE* arq, long r, Registro* reg, Estatisticas* est){
    if (fseek(arq, r * sizeof(Registro), SEEK_SET) != 0){
        printf("Erro ao posicionar arquivo para escrita.\n");
        return 0;
    }
    
    if (fwrite(reg, sizeof(Registro), 1, arq) != 1){
        printf("Erro ao escrever registro.\n");
        return 0;
    }
    
    if(est != NULL){
        est->escritas++;
    }
    return 1;
}

void inserePivo(FILE* arq, Estatisticas* est, Registro pivo[], int* N, long* EscritaEsq, Registro reg){
    int k = *N - 1;

    while (k >= 0){
        if (est) 
            est->comparacoes++;
        if (pivo[k].nota <= reg.nota) 
            break;
        pivo[k + 1] = pivo[k];
        k--;
    }

    pivo[k + 1] = reg;
    (*N)++;
}

int gravaPivo(FILE* arq, Registro pivo[], int N, long EscritaEsq, Estatisticas* est){
    for( int i = 0; i < N; i++){
        if((fseek(arq, EscritaEsq *sizeof(Registro), SEEK_SET)) != 0){ //Posiciona o arquivo
            printf("Erro ao posicionar arquivo.\n");
            return 0;
        }
        if((fwrite(&pivo[i], sizeof(Registro), 1, arq)) != 1){ //Grava o registro do pivo
            printf("Erro ao gravar o registro.\n");
            return 0; 
        }

        if(est != NULL){
            est->escritas++;
        }

        EscritaEsq++;
    }
    return 1;   
}

int removeMaiorPivo(FILE* arq, Registro pivo[], int* N, Estatisticas* est, long* EscritaDir, float* Lsup){
    if(*N <= 0){ //Verifica se existem registro disponiveis no pivo.
        return 0 ;
    }

    if((fseek(arq, (*EscritaDir) * sizeof(Registro), SEEK_SET)) != 0){ //Posiciona o arquivo 
        printf("Erro ao posicionar o arquivo.\n");
        return 0;
    }

    if((fwrite(&pivo[*N-1], sizeof(Registro), 1, arq)) != 1){ //Grava o maior registro
        printf("Erro ao registrar arquivo.\n");
        return 0;
    }

    if (est != NULL){
        est->escritas++;
    }

    *Lsup = pivo[*N-1].nota; 
    (*EscritaDir)--;
    (*N)--;

    return 1;
}

int removeMenorPivo(FILE* arq, Registro pivo[], int* N, Estatisticas* est, long* EscritaEsq, float* Linf){
    if(*N <= 0){
        return 0;
    }

    if(fseek(arq, (*EscritaEsq) * sizeof(Registro), SEEK_SET) != 0){
        printf("Erro ao posicionar o arquivo.\n");
        return 0;
    }

    if(fwrite(&pivo[0], sizeof(Registro), 1, arq) != 1){
        printf("Erro ao registrar arquivo.\n");
        return 0;
    }

    if (est != NULL){
        est->escritas++;
    }

    *Linf = pivo[0].nota; 
    (*EscritaEsq)++;

    for (int i = 0; i < *N - 1; i++) {
        pivo[i] = pivo[i + 1]; 
    }
    (*N)--;

    return 1;
}

int particiona(FILE* arq, long Esq, long Dir, long* i, long* j, Estatisticas* est){
    long LeituraEsq = Esq;
    long LeituraDir = Dir;

    long EscritaEsq = Esq;
    long EscritaDir = Dir;

    float Linf = -FLT_MAX;
    float Lsup = FLT_MAX;

    bool lerSuperior = true;
    int N = 0;
    Registro pivo[LIMITE_MEMORIA];
    Registro lido;

    // Enche a área do pivô (sem gravar nada)
    while (N < LIMITE_MEMORIA - 1 && LeituraEsq <= LeituraDir) {
        if (lerSuperior){
            if (!leRegistro(arq, LeituraDir, &lido, est)) return 0;
            LeituraDir--;
        }else{
            if (!leRegistro(arq, LeituraEsq, &lido, est)) return 0;
            LeituraEsq++;
        }

        lerSuperior = !lerSuperior;
        inserePivo(arq, est, pivo, &N, &EscritaEsq, lido);
    }

    // Processa o restante
    while (LeituraEsq <= LeituraDir){
        // evita sobrescrever registro ainda não lido
        if (LeituraDir == EscritaDir)      lerSuperior = true;
        else if (LeituraEsq == EscritaEsq) lerSuperior = false;

        if (lerSuperior){
            if (!leRegistro(arq, LeituraDir, &lido, est)) return 0;
            LeituraDir--;
        }else{
            if (!leRegistro(arq, LeituraEsq, &lido, est)) return 0;
            LeituraEsq++;
        }

        lerSuperior = !lerSuperior;

        est->comparacoes++;
        if (lido.nota > Lsup){
            if (!escreveRegistro(arq, EscritaDir, &lido, est)) return 0;
            EscritaDir--;
        }else{
            est->comparacoes++;
            if (lido.nota < Linf){
                if (!escreveRegistro(arq, EscritaEsq, &lido, est)) return 0;
                EscritaEsq++;
            }else{
                inserePivo(arq, est, pivo, &N, &EscritaEsq, lido);
                if (EscritaEsq - Esq < Dir - EscritaDir)
                    removeMenorPivo(arq, pivo, &N, est, &EscritaEsq, &Linf);
                else
                    removeMaiorPivo(arq, pivo, &N, est, &EscritaDir, &Lsup);
            }
        }
    }

    // limites das partições ANTES de gravar o pivô
    *i = EscritaEsq - 1;
    *j = EscritaDir + 1;

    return gravaPivo(arq, pivo, N, EscritaEsq, est);
}

void quicksort(FILE* arq, long n, Estatisticas* est){
    quicksortRec(arq, 0, n - 1, est);
}

void quicksortRec(FILE* arq, long Esq, long Dir, Estatisticas* est){

}

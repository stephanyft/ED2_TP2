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

void inserePivo(Estatisticas* est, Registro pivo[], int* N, Registro reg){
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
        inserePivo(est, pivo, &N, lido);
    }

    // Processa o restante
    while (LeituraEsq <= LeituraDir){

        // evita sobrescrever registro ainda não lido
        if (LeituraEsq == EscritaEsq) lerSuperior = false;
        else if (LeituraDir == EscritaDir) lerSuperior = true;
        
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
                inserePivo(est, pivo, &N, lido);

                
                long t1 = EscritaEsq - Esq;
                long t2 = Dir - EscritaDir;
                    
                
                if (t1 < t2)
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
      // Verifica se o arquivo é válido
    if (arq == NULL) {
        printf("Erro: arquivo invalido.\n");
        return;
    }
    //Verifica se existem resgistros para ordenar 
    if(n<= 1) return;

    quicksortRec(arq, 0, n - 1, est);
}

void quicksortRec(FILE* arq, long Esq, long Dir, Estatisticas* est){
    if( Esq >= Dir) return; //Condição de parada 
    long i, j; // Limites finais das regiões esquerda e direita
    if((particiona(arq, Esq, Dir, &i, &j, est)) != 1) return;

    if(i - Esq < Dir- j){
        //Primeiro ordena a esquerda
        quicksortRec(arq, Esq, i, est);
        //Depois ordena a direita
        quicksortRec(arq, j, Dir, est);
    }
    else{
        //primeiro ordena a direita
        quicksortRec(arq, j, Dir, est);

        //Depois ordena a esquerda
        quicksortRec(arq, Esq, i, est);
    }


}

int metodoQuicksortExterno(FILE* txt, int n, int imprimir, Estatisticas* est){
    const char* nomeBin = "quicksort.bin";
    const char* nomeSaida = "ordenado_quicksort.txt";
    Registro r;
    long k;
    int ok = 1;

    if (txt == NULL || n <= 0)
        return 0;

    FILE* bin = fopen(nomeBin, "w+b");
    if (bin == NULL){
        printf("Erro ao criar o arquivo binario temporario.\n");
        return 0;
    }

    /* Impressão antes de ordenar (fora do tempo e das estatísticas) */
    if (imprimir){
        printf("\n===== Dados antes de ordenar =====\n\n");
        fseek(txt, 0, SEEK_SET);
        for (k = 0; k < n && registroLeTexto(txt, &r, NULL); k++)
            registroImprime(&r);
        printf("\n==================================\n");
        fseek(txt, 0, SEEK_SET);
    }

    estatisticasIniciaTempo(est);

    /* Etapa 1: texto -> binário (conta leitura do texto e escrita no binário) */
    for (k = 0; k < n && ok; k++){
        if (!registroLeTexto(txt, &r, est)){
            printf("Erro: o arquivo possui menos de %d registros validos.\n", n);
            ok = 0;
        } else if (!escreveRegistro(bin, k, &r, est)){
            ok = 0;
        }
    }

    /* Etapa 2: ordenação */
    if (ok){
        quicksort(bin, n, est);
        fflush(bin);
    }

    /* Etapa 3: binário ordenado -> arquivo texto de saída */
    if (ok){
        FILE* saida = fopen(nomeSaida, "wb");
        if (saida == NULL){
            printf("\nFalha ao criar arquivo ordenado!\n");
            ok = 0;
        } else {
            for (k = 0; k < n; k++){
                if (!leRegistro(bin, k, &r, est)){
                    ok = 0;
                    break;
                }
                registroEscreveTexto(saida, &r, est);
            }
            fclose(saida);
        }
    }

    estatisticasParaTempo(est);

    /* Impressão depois de ordenar (fora do tempo e das estatísticas) */
    if (imprimir && ok){
        printf("\n===== Dados depois de ordenar =====\n\n");
        for (k = 0; k < n; k++){
            if (leRegistro(bin, k, &r, NULL))
                registroImprime(&r);
        }
        printf("\n==================================\n");
    }

    fclose(bin);
    remove(nomeBin);
    return ok;
}
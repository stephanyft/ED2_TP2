#include "../includes/quicksort.h"
#include <stdbool.h>
#include <float.h>

// Lê um registro do arquivo na posição especificada por 'r'
int leRegistro(FILE* arq, long r, Registro* reg, Estatisticas* est){
    // Calcula a posição do registro no arquivo e posiciona o ponteiro de leitura
    if (fseek(arq, r * sizeof(Registro), SEEK_SET) != 0){
        printf("Erro ao posicionar arquivo para leitura.\n");
        return 0;
    }

    // Lê o registro do arquivo e armazena no ponteiro 'reg'
    if (fread(reg, sizeof(Registro), 1, arq) != 1){
        return 0;
    }

    // Atualiza a quantidade de leituras, caso as estatísticas sejam fornecidas
    if (est != NULL){
        est->leituras++;
    }
    return 1;
}

// Escreve um registro no arquivo na posição especificada por 'r'
int escreveRegistro(FILE* arq, long r, Registro* reg, Estatisticas* est){
    // Posiciona o cursor no local onde o registro será escrito
    if (fseek(arq, r * sizeof(Registro), SEEK_SET) != 0){
        printf("Erro ao posicionar arquivo para escrita.\n");
        return 0;
    }

    // Escreve o registro no arquivo
    if (fwrite(reg, sizeof(Registro), 1, arq) != 1){
        printf("Erro ao escrever registro.\n");
        return 0;
    }

    // Registra a operacao de escrita
    if(est != NULL){
        est->escritas++;
    }
    return 1;
}

// Insere um registro no array de pivôs, mantendo a ordem crescente com base na nota
void inserePivo(Estatisticas* est, Registro pivo[], int* N, Registro reg){
    // Começa a comparacao pelo ultimo elemento armazenado
    int k = *N - 1;

    while (k >= 0){
        if (est) 
            est->comparacoes++;

        // Se encontrou uma posicao adequada, encerra o deslocamento
        if (pivo[k].nota <= reg.nota) 
            break;

        // Desloca o registro maior uma posicao para a direita
        pivo[k + 1] = pivo[k];
        k--;
    }

    // Insere o novo registro na posicao correta
    pivo[k + 1] = reg;

    // Atualiza a quantidade de elementos armazenados
    (*N)++;
}

// Grava os registros do array de pivôs no arquivo
int gravaPivo(FILE* arq, Registro pivo[], int N, long EscritaEsq, Estatisticas* est){
    for( int i = 0; i < N; i++){
        // Posiciona o arquivo na proxima posicao livre da região central
        if((fseek(arq, EscritaEsq *sizeof(Registro), SEEK_SET)) != 0){
            printf("Erro ao posicionar arquivo.\n");
            return 0;
        }

        // Grava o registro que estava armazenado em memoria
        if((fwrite(&pivo[i], sizeof(Registro), 1, arq)) != 1){
            printf("Erro ao gravar o registro.\n");
            return 0; 
        }

        if(est != NULL){
            est->escritas++;
        }

        // Avança para a proxima posicao de escrita
        EscritaEsq++;
    }
    return 1;   
}

// Remove o maior registro do array de pivôs
int removeMaiorPivo(FILE* arq, Registro pivo[], int* N, Estatisticas* est, long* EscritaDir, float* Lsup){
    // Verifica se há registros disponivel no array de pivôs
    if(*N <= 0){
        return 0 ;
    }

    // Posiciona o arquivo na última posição livre da região direita
    if((fseek(arq, (*EscritaDir) * sizeof(Registro), SEEK_SET)) != 0){
        printf("Erro ao posicionar o arquivo.\n");
        return 0;
    }

    // Grava o maior registro do array de pivôs na região direita
    if((fwrite(&pivo[*N-1], sizeof(Registro), 1, arq)) != 1){ 
        printf("Erro ao registrar arquivo.\n");
        return 0;
    }

    if (est != NULL){
        est->escritas++;
    }

    // Atualiza o limite superior com a nota do registro removido
    *Lsup = pivo[*N-1].nota; 

    // A região direita passa a ter uma nova posição ocupada
    (*EscritaDir)--;

    // Diminui a quantidade de registros armazenados no array de pivôs
    (*N)--;

    return 1;
}

// Remove o menor registro do array de pivôs
int removeMenorPivo(FILE* arq, Registro pivo[], int* N, Estatisticas* est, long* EscritaEsq, float* Linf){
    if(*N <= 0){
        return 0;
    }

    // Posiciona o arquivo na próxima posição livre da região esquerda
    if(fseek(arq, (*EscritaEsq) * sizeof(Registro), SEEK_SET) != 0){
        printf("Erro ao posicionar o arquivo.\n");
        return 0;
    }

    // Grava o menor registro do array de pivôs na região esquerda
    if(fwrite(&pivo[0], sizeof(Registro), 1, arq) != 1){
        printf("Erro ao registrar arquivo.\n");
        return 0;
    }

    if (est != NULL){
        est->escritas++;
    }

    // Atualiza o limite inferior com a nota do registro removido
    *Linf = pivo[0].nota; 

    // Avança a posição de escrita da região esquerda
    (*EscritaEsq)++;

    // Desloca os registros restantes no array de pivôs para a esquerda
    for (int i = 0; i < *N - 1; i++) {
        pivo[i] = pivo[i + 1]; 
    }

    // Atualiza a quantidade de registros armazenados no array de pivôs
    (*N)--;

    return 1;
}

// Particiona o arquivo em três regiões: menor ou igual ao limite inferior, igual ao pivô e maior ou igual ao limite superior
int particiona(FILE* arq, long Esq, long Dir, long* i, long* j, Estatisticas* est){
    // Limite utilizados para percorrer os registros ainda nao processados
    long LeituraEsq = Esq;
    long LeituraDir = Dir;

    // Limites utilizados para gravar os registros nas extremidades
    long EscritaEsq = Esq;
    long EscritaDir = Dir;

    // Limites utilizados para armazenar os registros do pivô
    float Linf = -FLT_MAX;
    float Lsup = FLT_MAX;

    // Controla de qual extremidade será feita a proxima leitura
    bool lerSuperior = true;

    // Quantidade atual de registros armazenados na area do pivô
    int N = 0;

    // Area de memoria limitada para armazenar os registros do pivô
    Registro pivo[LIMITE_MEMORIA];
    Registro lido;

    // Etapa 1: Preenche a area do pivô com registros do arquivo alternando entre as extremidadesaté atingir o limite de memória ou até que todos os registros tenham sido lidos
    while (N < LIMITE_MEMORIA - 1 && LeituraEsq <= LeituraDir) {
        if (lerSuperior){
            // Lê um registro da extremidade direita ainda nao processado
            if (!leRegistro(arq, LeituraDir, &lido, est)) return 0;
            LeituraDir--;
        }else{
            // Lê um registro da extremidade esquerda ainda nao processado
            if (!leRegistro(arq, LeituraEsq, &lido, est)) return 0;
            LeituraEsq++;
        }

        // Alterna a extremidade de leitura 
        lerSuperior = !lerSuperior;

        // Insere o registro lido na area do pivô
        inserePivo(est, pivo, &N, lido);
    }

    // Etapa 2: Processa os registros restantes do arquivo, comparando-os com os limites inferior e superior e gravando-os nas regiões apropriadas ou na área do pivô
    while (LeituraEsq <= LeituraDir){

        // Verifica se a proxima leitura deve ser feita da extremidade esquerda ou direita, caso uma das extremidades já tenha sido totalmente processada
        if (LeituraEsq == EscritaEsq) lerSuperior = false;
        else if (LeituraDir == EscritaDir) lerSuperior = true;
        
        // Realiza a leitura pela extremidade escolhida e atualiza os limites de leitura
        if (lerSuperior){
            if (!leRegistro(arq, LeituraDir, &lido, est)) return 0;
            LeituraDir--;
        }else{
            if (!leRegistro(arq, LeituraEsq, &lido, est)) return 0;
            LeituraEsq++;
        }

        // Alterna a extremidade de leitura para a próxima iteração
        lerSuperior = !lerSuperior;

        // Incrementa o contador de comparações
        est->comparacoes++;

        if (lido.nota > Lsup){
            // Se o registro lido for maior que o limite superior, grava-o na região direita
            if (!escreveRegistro(arq, EscritaDir, &lido, est)) return 0;
            EscritaDir--;
        }else{
            // Incrementa o contador de comparações
            est->comparacoes++;

            // Se o registro lido for menor que o limite inferior, grava-o na região esquerda
            if (lido.nota < Linf){
                if (!escreveRegistro(arq, EscritaEsq, &lido, est)) return 0;
                EscritaEsq++;

            }else{
                // Se o registro lido estiver entre os limites, insere-o na área do pivô
                inserePivo(est, pivo, &N, lido);

                // Calcula o tamanho das regiões esquerda e direita já preenchidas
                long t1 = EscritaEsq - Esq;
                long t2 = Dir - EscritaDir;
                    
                // Remove o menor ou maior registro do pivô, dependendo de qual região está menor, para manter o equilíbrio entre as regiões esquerda e direita
                if (t1 < t2)
                    removeMenorPivo(arq, pivo, &N, est, &EscritaEsq, &Linf);
                else
                    removeMaiorPivo(arq, pivo, &N, est, &EscritaDir, &Lsup);
            }
        }
    }

    // Etapa 3: Quando todos os registros foram lidos, calculamos os limites das regiões esquerda e direita e gravamos os registros restantes do pivô no arquivo
    *i = EscritaEsq - 1;
    *j = EscritaDir + 1;

    return gravaPivo(arq, pivo, N, EscritaEsq, est);
}

// Função principal do algoritmo de ordenação rápida (quicksort) que ordena os registros em um arquivo binário
void quicksort(FILE* arq, long n, Estatisticas* est){
    // Verifica se o arquivo é válido
    if (arq == NULL) {
        printf("Erro: arquivo invalido.\n");
        return;
    }

    //Verifica se existem resgistros para ordenar 
    if(n<= 1) return;

    // Inicia a ordenação considerando todo o arquivo
    quicksortRec(arq, 0, n - 1, est);
}

// Função recursiva do quicksort que ordena os registros em um arquivo binário entre os índices Esq e Dir
void quicksortRec(FILE* arq, long Esq, long Dir, Estatisticas* est){
    // Condição de parada: região vazia ou com apenas um registro.
    if(Esq >= Dir) return;

    // Armazena os limites das duas partições.
    long i, j;

    // Realiza a partição e verifica se ocorreu algum erro.
    if((particiona(arq, Esq, Dir, &i, &j, est)) != 1) return;

    // Compara o tamanho das duas partições e ordena a menor primeiro para otimizar o uso da pilha de chamadas recursivas.
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

// Função que implementa o método de ordenação externa utilizando o algoritmo quicksort
int metodoQuicksortExterno(FILE* txt, int n, int imprimir, Estatisticas* est){
    // Define os nomes dos arquivos
    const char* nomeBin = "quicksort.bin";
    const char* nomeSaida = "ordenado_quicksort.txt";

    Registro r;
    long k;
    int ok = 1;

    // Verifica se o arquivo de entrada é válido
    if (txt == NULL || n <= 0)
        return 0;

    // Cria o arquivo binário temporário para armazenar os registros
    FILE* bin = fopen(nomeBin, "w+b");
    if (bin == NULL){
        printf("Erro ao criar o arquivo binario temporario.\n");
        return 0;
    }

    // Impressão antes de ordenar (fora do tempo e das estatísticas)
    if (imprimir){
        printf("\n===== Dados antes de ordenar =====\n\n");
        
        // Volta ao inicio do arquivo de texto
        fseek(txt, 0, SEEK_SET);
        
        for (k = 0; k < n && registroLeTexto(txt, &r, NULL); k++)
            registroImprime(&r);
        
        printf("\n==================================\n");
        
        // Reposiciona o arquivo para iniciar a leitura dos dados
        fseek(txt, 0, SEEK_SET);
    }

    // Inicia a contagem do tempo de pré-processamento
    estatisticasIniciaTempoPre(est);

    //Etapa 1: texto -> binário
    for (k = 0; k < n && ok; k++){
        // Le um registro do arquivo de texto
        if (!registroLeTexto(txt, &r, est)){
            printf("Erro: o arquivo possui menos de %d registros validos.\n", n);
            ok = 0;
        } else if (!escreveRegistro(bin, k, &r, est)){
            // Grava o registro na posicao correspondente do arquivo binário
            ok = 0;
        }
    }

    // Finaliza a medicao do tempo de pré-processamento
    estatisticasParaTempoPre(est);

    // Etapa 2: ordenação
    if (ok){
        estatisticasIniciaTempoOrd(est);
        quicksort(bin, n, est);
        fflush(bin);
        estatisticasParaTempoOrd(est);
    }

    // Etapa 3: binário ordenado -> arquivo texto de saída
    if (ok){
        FILE* saida = fopen(nomeSaida, "wb");
        
        if (saida == NULL){
            printf("\nFalha ao criar arquivo ordenado!\n");
            ok = 0;
        } else {
            for (k = 0; k < n; k++){
                // Recupera cada registro na ordem em que foi organizado
                if (!leRegistro(bin, k, &r, est)){
                    ok = 0;
                    break;
                }

                // Escreve o registro no arquivo de saída
                registroEscreveTexto(saida, &r, est);
            }
            fclose(saida);
        }
    }

    // Impressão depois de ordenar (fora do tempo e das estatísticas)
    if (imprimir && ok){
        printf("\n===== Dados depois de ordenar =====\n\n");
        
        for (k = 0; k < n; k++){
            if (leRegistro(bin, k, &r, NULL))
                registroImprime(&r);
        }
        
        printf("\n==================================\n");
    }

    // Fecha o arquivo binário e remove-o do sistema
    fclose(bin);
    remove(nomeBin);
    return ok;
}
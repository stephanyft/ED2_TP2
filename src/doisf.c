#include "../includes/doisf.h"
#include "subSelecao.h"

int metodoDoisFCaminhos(FILE *arq, int n, int imprimir, Estatisticas *est) {
    if(arq == NULL)
        return 0;
    
    Fita* f[20];
    
    //abre as 20 fitas auxiliares
    for (int i = 0; i < 20; i++) {
        f[i] = malloc(sizeof(Fita));

        if (f[i] == NULL)
            return 0;

        fitaAbre(f[i], i + 1);
    }

    //imprime os dados antes de ordenar
    if (imprimir) {
        Registro tmp;
        printf("\n===== Dados antes de ordenar =====\n\n");
        fseek(arq, 0, SEEK_SET);
        for (int i = 0; i < n && registroLeTexto(arq, &tmp, NULL); i++)
            registroImprime(&tmp);
        printf("\n==================================\n");
        fseek(arq, 0, SEEK_SET);
    }
    
    //inicia o metodo 2f caminhos
    estatisticasIniciaTempoPre(est);
    ordenaBlocosIniciais(arq, n, f, est);
    estatisticasParaTempoPre(est);

    estatisticasIniciaTempoOrd(est);
    int num = intercalaFitas(f, est);
    estatisticasParaTempoOrd(est);
    
    if (num < 0) {
        return 0;
    }

    //imprime os dados ordenados
    if(imprimir) {
        printf("\n===== Dados depois de ordenar =====\n\n");
        fitaImprime(f[num]);
        printf("\n==================================\n");
    }
    
    //gera o arquivo com os dados ordenados
    Registro r;
    FILE *arq2 = fopen("ordenado_doisf.txt", "wb");
    if(arq2 == NULL) {
        printf("\nFalha ao criar arquivo ordenado!\n");
    }
    else {
        fitaRebobina(f[num]);
        while (fitaLe(f[num], &r, NULL)) {
            registroEscreveTexto(arq2, &r, NULL);
        }
        fclose(arq2);
    }

    //fecha as fitas auxiliares
    for (int i = 0; i < 20; i++) {
        fitaFecha(f[i]);
        free(f[i]);
    }

    return 1;
}

int ordenaBlocosIniciais(FILE *arq, int n, Fita* f[], Estatisticas *est) {
    Heap h;
    Registro reg;

    fseek(arq, 0, SEEK_SET);

    int lidos = 0;
    int cont = 0;
    int qtdHeap = 0;
    int fitaAtual = 0;

    //le os 10 primeiros registros do arquivo e os armazena no heap
    while (qtdHeap < 10 && lidos < n && registroLeTexto(arq, &reg, est) == 1) {

        h.v[qtdHeap].reg = reg;
        h.v[qtdHeap].marcado = false;

        qtdHeap++;
        lidos++;
    }

    constroiHeap(&h, qtdHeap, est);

    //laço que inicia, escreve e finaliza os blocos inicialmente ordenados por meio do heap
    while (qtdHeap > 0) {
        fitaIniciaBloco(f[fitaAtual]);
        escreveBloco(arq, f[fitaAtual], &h, &cont, &lidos, n, &qtdHeap, est);
        fitaFinalizaBloco(f[fitaAtual]);
        fitaAtual = (fitaAtual + 1) % 10;
    }

    return cont == n;
}

void escreveBloco(FILE *arq, Fita *f, Heap *h, int *cont, int *lidos, int n, int *qtdHeap, Estatisticas *est) {
    Registro novo;
    Registro ultimo;

    preparaHeap(h, *qtdHeap, est);

    while (*qtdHeap > 0) {
        //se todos os itens estiverem marcados, finaliza a funçao
        if (fimBloco(h, *qtdHeap))
            break;

        //atualiza a fita origem do menor registro do heap
        h->v[0].fitaOrigem = f->indice;
        ultimo = h->v[0].reg;        

        //escreve o menor registro do heap na fita de saida
        fitaEscreve(f, &ultimo, est);
        (*cont)++;

        //le o proximo item do arquivo
        if ((*lidos) < n && registroLeTexto(arq, &novo, est) == 1) {
            (*lidos)++;

            //troca a raiz do heap pelo novo registro, e verifica se ele sera marcado
            h->v[0].reg = novo;
            h->v[0].marcado = comparaNota(est, &novo, &ultimo) < 0 ? true : false;
        } 
        else { //quando todos os registros sao lidos
            h->v[0] = h->v[*qtdHeap - 1]; //troca a raiz pelo ultimo item do heap
            (*qtdHeap)--;   //diminui o tamanho do heap
        }

        if (*qtdHeap > 0)
            reconstroiHeap(h, 0, *qtdHeap, est);
    }
}

//funçao que verifica se as fitas ainda tem blocos a serem lidos
bool verificaSeTemBloco(Fita* f[], int inicioEntrada) {
    for(int i = inicioEntrada; i < (inicioEntrada + 10); i++) {
        if(fitaTemBloco(f[i])) {
            return true;
        }
    }
    return false;
}

//funçao que verifica se apenas uma fita ainda tem blocos a serem lidos
bool ultimoBloco(Fita* f[], int inicioSaida) {
    int cont = 0;
    for(int i = inicioSaida; i < (inicioSaida + 10); i++) {
        if(fitaTemBloco(f[i])) {
            cont++;
        }
    }
    if(cont != 1)
        return false;
    return true;
}

//funçao que retorna o indice da unica fita com blocos a serem lidos
int unicaFitaComBloco(Fita* f[], int inicio) {
    int achada = -1;
    for (int i = inicio; i < inicio + 10; i++) {
        if (fitaTemBloco(f[i])) {
            if (achada >= 0)
                return -1;
            achada = i;
        }
    }
    return achada;
}

int intercalaFitas(Fita* f[], Estatisticas* est) {
    if(f == NULL) {
        printf("erro na função intercalaFitas\n");
        return -1;
    }

    Heap h;
    Registro r;
    int inicioEntrada = 0, inicioSaida = 10;

    //rebobina as fitas de leitura
    for(int i = 0; i < 10; i++) {
        fitaRebobina(f[i]);
    }

    //reinicia as fitas de saida
    for(int j = 10; j < 20; j++) {
        fitaReinicia(f[j]);
    }

    //verifica se foi formado apenas um bloco incialmente ordenado
    if(ultimoBloco(f, inicioEntrada)) {
      for (int i = inicioEntrada; i < inicioEntrada + 10; i++)
                if (fitaTemBloco(f[i])) { 
                    fitaRebobina(f[i]);     //retorna o indice da fita com o bloco ordenado
                    return i;
                }
    }
    
    
    int fitaSaidaAtual = -1;
    int qtdHeap = 0;
    int itensLidos = 0;
    while(1) {
        //verifica se ainda tem blocos nas fitas de entradas para serem intercalados
        while(verificaSeTemBloco(f, inicioEntrada)) {

            //Verifica se apenas uma fita de entrada ainda tem blocos
            int unica = unicaFitaComBloco(f, inicioEntrada);

            if (unica >= 0) { //Copia a unica fita que ainda tem bloco diretamente
                while (fitaTemBloco(f[unica])) {
                    fitaSaidaAtual = (fitaSaidaAtual + 1) % 10;
                    if (inicioSaida == 10)
                        fitaSaidaAtual += 10;
                    fitaIniciaBloco(f[fitaSaidaAtual]);
                    while (fitaLe(f[unica], &r, est))
                        fitaEscreve(f[fitaSaidaAtual], &r, est);
                    fitaFinalizaBloco(f[fitaSaidaAtual]);
                    fitaProximoBloco(f[unica]);
                }
                break;
            }

            //Copia os 10 proximos itens dos proximos blocos
            //para o heap em memoria interna e constroi o heap
            for(int k = inicioEntrada; k < (inicioEntrada + 10); k++) {
                if(fitaTemBloco(f[k]) == 1) {
                    if(fitaLe(f[k], &r, est) == 1) {
                        h.v[qtdHeap].reg = r;
                        h.v[qtdHeap].fitaOrigem = k;
                        h.v[qtdHeap].marcado = false;
                        qtdHeap++;
                        itensLidos++;
                    } 
                }
            }
            constroiHeap(&h, qtdHeap, est);
            
            //calcula qual sera a proxima fita de saida para escrita
            fitaSaidaAtual = (fitaSaidaAtual + 1) % 10;
            if(inicioSaida == 10)
                fitaSaidaAtual += 10;
            fitaIniciaBloco(f[fitaSaidaAtual]);
            
            //Laço de intercalaçao principal
            //enquanto ainda houver itens no heap, continua a intercalar
            while(qtdHeap > 0) {
                //escreve o menor item na fita de saida e le o proximo item
                fitaEscreve(f[fitaSaidaAtual], &(h.v[0].reg), est);
                int resultLeitura = fitaLe(f[h.v[0].fitaOrigem], &r, est);
                
                //verifica se o bloco daquela fita ja acabou
                if(!resultLeitura) {
                    h.v[0] = h.v[qtdHeap-1];
                    qtdHeap--;                 //decrementa a quantidade de itens no heap
                    reconstroiHeap(&h, 0, qtdHeap, est);
                    continue;
                }
                
                substituiRaiz(&h, r, h.v[0].reg, qtdHeap, est);
                itensLidos++;
            }
            
            //finaliza o bloco da fita de saida e avança para os proximos blocos de leitura
            fitaFinalizaBloco(f[fitaSaidaAtual]);
            for(int l = inicioEntrada; l < (inicioEntrada + 10); l++) {
                fitaProximoBloco(f[l]);
            }
            
        }

        //verifica se a intercalaçao ja terminou
        if (ultimoBloco(f, inicioSaida)) {
            for (int i = inicioSaida; i < inicioSaida + 10; i++)
                if (fitaTemBloco(f[i])) { 
                    fitaRebobina(f[i]);         //retorna o indice da fita com o bloco ordenado
                    return i;
                }
        }

        //rebobina as fitas de saida para a proxima intercalaçao
        for(int i = inicioSaida; i < (inicioSaida + 10); i++) {
            fitaRebobina(f[i]);
        }
        //reinicia as fitas de entrada para a proxima intercalaçao
        for (int i = inicioEntrada; i < inicioEntrada + 10; i++) {
            fitaReinicia(f[i]);
        }

        //troca os indices das fitas de entrada e saida para a proxima intercalaçao
        int temp = inicioEntrada;
        inicioEntrada = inicioSaida;
        inicioSaida = temp;
    }
}
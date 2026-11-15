#include <stdlib.h>
#include <string.h>
#include "registro.h"

/* Copia n caracteres de origem para destino, remove espacos finais e termina com '\0'. */
static void copiaCampo(char *destino, const char *origem, int n) {
    memcpy(destino, origem, (size_t)n);
    destino[n] = '\0';
    while (n > 0 && destino[n - 1] == ' ') {
        destino[--n] = '\0';
    }
}

int registroDeLinha(const char *linha, Registro *r) {
    char numero[16];

    if (strlen(linha) < TAM_LINHA_TEXTO) {
        return 0;
    }

    /* inscricao: colunas 1 a 8 -> indices 0 a 7 */
    memcpy(numero, linha, 8);
    numero[8] = '\0';
    r->inscricao = atol(numero);

    /* nota: colunas 10 a 14 -> indices 9 a 13 */
    memcpy(numero, linha + 9, 5);
    numero[5] = '\0';
    r->nota = (float)atof(numero);

    copiaCampo(r->estado, linha + 15, 2);   /* colunas 16 a 17 */
    copiaCampo(r->cidade, linha + 18, 50);  /* colunas 19 a 68 */
    copiaCampo(r->curso,  linha + 69, 30);  /* colunas 70 a 99 */

    return 1;
}

int registroLeTexto(FILE *arq, Registro *r, Estatisticas *est) {
    char linha[TAM_BUFFER_LINHA];

    if (fgets(linha, sizeof linha, arq) == NULL) {
        return 0;
    }
    if (!registroDeLinha(linha, r)) {
        return 0;
    }
    if (est != NULL) {
        est->leituras++;
    }
    return 1;
}

void registroEscreveTexto(FILE *arq, const Registro *r, Estatisticas *est) {
    fprintf(arq, "%08ld %05.1f %-2s %-50s %-30s\r\n",
            r->inscricao, r->nota, r->estado, r->cidade, r->curso);
    if (est != NULL) {
        est->escritas++;
    }
}

void registroImprime(const Registro *r) {
    printf("%08ld  %5.1f  %-2s  %-50s  %-30s\n",
           r->inscricao, r->nota, r->estado, r->cidade, r->curso);
}

int comparaNota(Estatisticas *est, const Registro *a, const Registro *b) {
    if (est != NULL) {
        est->comparacoes++;
    }
    if (a->nota < b->nota) return -1;
    if (a->nota > b->nota) return 1;
    return 0;
}

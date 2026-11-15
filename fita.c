#include <stdlib.h>
#include <string.h>
#include "fita.h"

static void erroFatal(const char *msg, const char *nome) {
    fprintf(stderr, "Erro: %s (%s)\n", msg, nome);
    exit(EXIT_FAILURE);
}

static void zeraBlocos(Fita *f) {
    f->numBlocos = 0;
    f->blocoAberto = 0;
    f->blocoAtual = 0;
    f->lidosNoBloco = 0;
}

void fitaAbre(Fita *f, int indice) {
    f->indice = indice;
    snprintf(f->nome, sizeof f->nome, "fita%02d.tmp", indice);
    f->arq = fopen(f->nome, "wb+");
    if (f->arq == NULL) {
        erroFatal("nao foi possivel criar arquivo temporario da fita", f->nome);
    }
    f->capBlocos = 64;
    f->tamBlocos = malloc((size_t)f->capBlocos * sizeof(long));
    if (f->tamBlocos == NULL) {
        erroFatal("sem memoria para a tabela de blocos", f->nome);
    }
    zeraBlocos(f);
}

void fitaFecha(Fita *f) {
    if (f->arq != NULL) {
        fclose(f->arq);
        remove(f->nome);
        f->arq = NULL;
    }
    free(f->tamBlocos);
    f->tamBlocos = NULL;
}

void fitaReinicia(Fita *f) {
    /* freopen com "wb+" trunca o arquivo e reposiciona no inicio */
    f->arq = freopen(f->nome, "wb+", f->arq);
    if (f->arq == NULL) {
        erroFatal("nao foi possivel reiniciar a fita", f->nome);
    }
    zeraBlocos(f);
}

/* ---- escrita ---- */

void fitaIniciaBloco(Fita *f) {
    if (f->blocoAberto) {
        erroFatal("fitaIniciaBloco chamado com bloco ja aberto", f->nome);
    }
    if (f->numBlocos == f->capBlocos) {
        f->capBlocos *= 2;
        f->tamBlocos = realloc(f->tamBlocos, (size_t)f->capBlocos * sizeof(long));
        if (f->tamBlocos == NULL) {
            erroFatal("sem memoria para a tabela de blocos", f->nome);
        }
    }
    f->tamBlocos[f->numBlocos] = 0;
    f->blocoAberto = 1;
}

void fitaEscreve(Fita *f, const Registro *r, Estatisticas *est) {
    if (!f->blocoAberto) {
        erroFatal("fitaEscreve chamado sem bloco aberto", f->nome);
    }
    if (fwrite(r, sizeof(Registro), 1, f->arq) != 1) {
        erroFatal("falha de escrita na fita", f->nome);
    }
    f->tamBlocos[f->numBlocos]++;
    if (est != NULL) {
        est->escritas++;
    }
}

void fitaFinalizaBloco(Fita *f) {
    if (!f->blocoAberto) {
        erroFatal("fitaFinalizaBloco chamado sem bloco aberto", f->nome);
    }
    f->blocoAberto = 0;
    if (f->tamBlocos[f->numBlocos] > 0) {
        f->numBlocos++;
    }
}

/* ---- leitura ---- */

void fitaRebobina(Fita *f) {
    if (f->blocoAberto) {
        erroFatal("fitaRebobina chamado com bloco aberto", f->nome);
    }
    fflush(f->arq);
    rewind(f->arq);
    f->blocoAtual = 0;
    f->lidosNoBloco = 0;
}

int fitaLe(Fita *f, Registro *r, Estatisticas *est) {
    if (!fitaTemBloco(f) || f->lidosNoBloco >= f->tamBlocos[f->blocoAtual]) {
        return 0;
    }
    if (fread(r, sizeof(Registro), 1, f->arq) != 1) {
        erroFatal("falha de leitura na fita", f->nome);
    }
    f->lidosNoBloco++;
    if (est != NULL) {
        est->leituras++;
    }
    return 1;
}

int fitaProximoBloco(Fita *f) {
    if (!fitaTemBloco(f)) {
        return 0;
    }
    /* pula o que restou do bloco atual, caso nao tenha sido lido ate o fim */
    long restantes = f->tamBlocos[f->blocoAtual] - f->lidosNoBloco;
    if (restantes > 0) {
        fseek(f->arq, restantes * (long)sizeof(Registro), SEEK_CUR);
    }
    f->blocoAtual++;
    f->lidosNoBloco = 0;
    return fitaTemBloco(f);
}

int fitaTemBloco(const Fita *f) {
    return f->blocoAtual < f->numBlocos;
}

long fitaNumBlocos(const Fita *f) {
    return f->numBlocos;
}

long fitaTamBloco(const Fita *f, long i) {
    return (i >= 0 && i < f->numBlocos) ? f->tamBlocos[i] : 0;
}

long fitaTotalRegistros(const Fita *f) {
    long total = 0;
    for (long i = 0; i < f->numBlocos; i++) {
        total += f->tamBlocos[i];
    }
    return total;
}

void fitaImprime(Fita *f) {
    Registro r;
    fitaRebobina(f);
    while (fitaTemBloco(f)) {
        while (fitaLe(f, &r, NULL)) {
            registroImprime(&r);
        }
        fitaProximoBloco(f);
    }
}

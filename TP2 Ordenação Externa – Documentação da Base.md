# TP2 Ordenação Externa – Documentação da Base

Nov 13, 2026 · @gaiola de ouro

## Visão geral

O projeto implementa três métodos de ordenação externa em C sobre o arquivo PROVAO.TXT (471.705 registros), medindo leituras, escritas, comparações e tempo. Os três métodos compartilham uma base de três módulos (estatisticas, registro, fita) e um programa principal; cada método fica em um arquivo próprio e conversa com a base só pelas interfaces públicas dos cabeçalhos.

Restrições do enunciado que moldam a base:

- A chave de ordenação é a nota (ordem ascendente). Os registros têm tamanho fixo: inscrição, nota, estado, cidade e curso.
- A memória interna é limitada por método: 10 registros (método 1), 19 registros (método 2), 10 registros (método 3). A base nunca guarda registros além do que o método pede.
- Todo o programa deve relatar leituras (externa para interna), escritas (interna para externa), comparações entre chaves e tempo de execução. A base centraliza essa contagem para que nenhum método a faça por conta própria.
- Chamada: `ordena <método> <quantidade> <situação> [-P]`, com método 1 a 3, situação 1 ascendente, 2 descendente, 3 aleatório, e `-P` imprimindo entrada e resultado.

Divisão de trabalho: a base está em `main`; cada método é desenvolvido em uma branch própria e integrado quando passa nos seis tamanhos do experimento (100, 200, 2.000, 20.000, 200.000, 471.705) nas três situações.

## Mapa de arquivos e dependências

&#91;embedded content: dependências entre arquivos · base embaixo, métodos em cima\]

As dependências só apontam para baixo: estatisticas não inclui ninguem, registro inclui estatisticas, fita inclui os dois, e os métodos incluem o que precisam. Os métodos 1 e 2 usam seleção por substituição e fita; o método 3 usa acesso posicional. Todo arquivo que lê, grava ou compara recebe o ponteiro `Estatisticas` e é a base, não o método, que incrementa os contadores.

## Módulo estatisticas (estatisticas.h / estatisticas.c)

Guarda os quatro quesitos exigidos pelo trabalho em uma única struct, criada no `main` e passada por ponteiro para toda função que faz I/O de registro ou compara chaves. Não depende de nenhum outro módulo.

| Campo | Significado |
| --- | --- |
| `leituras` | registros transferidos da memória externa para a interna |
| `escritas` | registros transferidos da memória interna para a externa |
| `comparacoes` | comparações entre notas |
| `inicio`, `fim` | instantes do relógio monótono (`struct timespec`) |

| Função | O que faz |
| --- | --- |
| `estatisticasInicia` | zera contadores e instantes |
| `estatisticasIniciaTempo` / `estatisticasParaTempo` | marcam início e fim com `clock_gettime(CLOCK_MONOTONIC)` |
| `estatisticasTempo` | devolve o tempo decorrido em segundos |
| `estatisticasImprime` | imprime os quatro quesitos no formato final do programa |

Regra do módulo: nenhum método incrementa os contadores diretamente. Quem conta são as funções de registro e fita, que recebem o ponteiro `est`. Passar `NULL` desliga a contagem, o que serve para pré-processamento e impressão com `-P`.

Compilação: `clock_gettime` exige `-D_POSIX_C_SOURCE=200809L` (ou `-std=gnu11`) na linha do gcc. Isso deve entrar no Makefile.

## Módulo registro (registro.h / registro.c)

Define o `Registro`, a conversão entre uma linha do PROVAO.TXT e a struct, e a única função de comparação de chaves do projeto. Depende só de estatisticas.

Formato da linha: 99 caracteres úteis seguidos de `\r\n`, campos de largura fixa separados por um espaço. As posições em C são as colunas do enunciado menos 1.

| Campo | Colunas (enunciado) | Início em C | Tamanho | Tipo na struct |
| --- | --- | --- | --- | --- |
| inscrição | 1 a 8 | 0 | 8 | `long inscricao` |
| nota | 10 a 14 | 9 | 5 | `float nota` (chave) |
| estado | 16 a 17 | 15 | 2 | `char estado[3]` |
| cidade | 19 a 68 | 18 | 50 | `char cidade[51]` |
| curso | 70 a 99 | 69 | 30 | `char curso[31]` |

Os vetores de texto têm uma posição a mais para o `\0`. Espaços à direita são removidos na leitura e recolocados na escrita, então ler e escrever reproduz a linha original.

| Função | O que faz | Conta |
| --- | --- | --- |
| `registroDeLinha(linha, r)` | converte uma string de 99+ caracteres em `Registro`; devolve 0 se a linha for curta | nada |
| `registroLeTexto(arq, r, est)` | lê a próxima linha com `fgets` e converte; devolve 0 no fim do arquivo ou em linha inválida | 1 leitura |
| `registroEscreveTexto(arq, r, est)` | grava o registro em texto no formato do PROVAO.TXT | 1 escrita |
| `registroImprime(r)` | imprime na tela, uma linha por registro | nada |
| `comparaNota(est, a, b)` | devolve negativo, zero ou positivo comparando `a->nota` e `b->nota` | 1 comparação |

Quem usa o quê:

- Os métodos usam `registroLeTexto` para consumir a entrada na fase 1 e `comparaNota` em todo heap, intercalação ou partição. Nenhum método compara `nota` com `<` diretamente, senão a contagem fica errada.
- O pré-processamento usa `registroLeTexto` e `registroEscreveTexto` com `est = NULL` para gerar os arquivos ascendente e descendente.
- O `-P` usa `registroImprime`.

O retorno 0 de `registroLeTexto` é o sinal de fim de entrada para os métodos. O limite de `n` registros não é imposto aqui: cada método conta quantos leu e para em `n`, mesmo que o arquivo continue.

## Módulo fita (fita.h / fita.c)

Uma `Fita` simula um dispositivo sequencial: grava-se registro atrás de registro, rebobina-se e lê-se do início. Não há acesso por posição. No disco é um arquivo binário temporário (`fitaNN.tmp`) de structs `Registro` gravadas com `fwrite`, apagado ao fechar. Depende de registro e estatisticas.

O conteúdo é uma sequência de blocos (corridas ordenadas). O arquivo não tem marcador de fim de bloco; a fita guarda em `tamBlocos[i]` quantos registros o bloco `i` tem. Assim o leitor sabe onde um bloco termina sem comparar chaves, o que evita o erro clássico de dois blocos vizinhos "colarem" quando o primeiro registro do segundo é maior que o último do primeiro. Esse vetor é metadado, não registro, e não entra no limite de memória interna.

| Campo | Papel |
| --- | --- |
| `arq`, `nome`, `indice` | o arquivo físico e seu número |
| `numBlocos`, `tamBlocos`, `capBlocos` | tabela de tamanhos dos blocos (cresce dobrando a partir de 64) |
| `blocoAberto` | 1 enquanto um bloco está sendo gravado |
| `blocoAtual`, `lidosNoBloco` | cursor de leitura: bloco corrente e quantos registros dele já saíram |

Ciclo de vida:

| Função | O que faz |
| --- | --- |
| `fitaAbre(f, indice)` | cria o arquivo com `wb+` e a tabela de blocos vazia |
| `fitaReinicia(f)` | trunca o arquivo e zera a tabela; usada antes de reescrever uma fita em uma nova passada |
| `fitaFecha(f)` | fecha, remove o arquivo do disco e libera a tabela |

Escrita, sempre dentro de um bloco:

| Função | O que faz | Conta |
| --- | --- | --- |
| `fitaIniciaBloco(f)` | abre um bloco novo; erro fatal se já houver um aberto | nada |
| `fitaEscreve(f, r, est)` | grava `r` no bloco aberto; erro fatal sem bloco aberto | 1 escrita |
| `fitaFinalizaBloco(f)` | fecha o bloco e registra seu tamanho; bloco vazio é descartado | nada |

Leitura, bloco a bloco:

| Função | O que faz | Conta |
| --- | --- | --- |
| `fitaRebobina(f)` | `fflush` + `rewind`, cursor no bloco 0; erro fatal com bloco aberto | nada |
| `fitaLe(f, r, est)` | lê o próximo registro do bloco atual; devolve 0 quando o bloco acabou | 1 leitura |
| `fitaProximoBloco(f)` | avança o cursor (pula com `fseek` o que não foi lido); devolve 0 se não há mais blocos | nada |
| `fitaTemBloco(f)` | 1 se o cursor aponta para um bloco válido | nada |

Consultas sem I/O: `fitaNumBlocos`, `fitaTamBloco(f, i)`, `fitaTotalRegistros`. `fitaImprime` percorre a fita inteira com `est = NULL`, para o `-P`.

Regras de uso que o módulo fiscaliza com erro fatal:

- Em uma passada, uma fita ou está sendo escrita ou lida, nunca as duas coisas. Para passar de leitura a escrita, `fitaReinicia`.
- `fitaLe` devolve 0 no fim do **bloco**, não da fita. Isso é o que torna a intercalação simples: cada fita contribui um bloco por rodada; `fitaProximoBloco` só é chamada quando começa a rodada seguinte.
- O critério de parada dos métodos 1 e 2 é consultar `fitaNumBlocos` da fita que recebeu a última passada: igual a 1 significa arquivo ordenado.

## Como os três métodos usam a base

Cada método é uma função com a mesma assinatura, declarada em um cabeçalho comum (`metodos.h`) e implementada em seu próprio arquivo (`metodo1.c`, `metodo2.c`, `metodo3.c`). O `main` escolhe qual chamar pelo argumento e não sabe nada do que acontece dentro.

Contrato proposto: `void metodoN(FILE *entrada, long n, Estatisticas *est, Fita *saida)`. O método lê até `n` registros de `entrada` com `registroLeTexto`, ordena, e deixa o resultado em `saida` como um único bloco. Se o resultado terminar em outra fita, o método troca as structs `Fita` (custo zero de I/O). O `main` então imprime as estatísticas e, com `-P`, percorre `saida` com `fitaImprime`.

|  | Método 1: intercalação 2f | Método 2: intercalação f+1 | Método 3: quicksort externo |
| --- | --- | --- | --- |
| Memória interna | 10 registros | 19 registros | 10 registros |
| Fitas | 20 (10 entrada + 10 saída) | 20 (19 entrada + 1 saída) | nenhuma; acesso posicional |
| Fase 1 | seleção por substituição, heap de 10, blocos distribuídos nas 10 fitas de entrada | seleção por substituição, heap de 19, blocos distribuídos nas 19 fitas de entrada | não há |
| Fase 2 | intercala um bloco de cada fita de entrada para uma fita de saída, alternando entre as saídas; depois troca os papéis dos grupos | intercala um bloco de cada uma das 19 na única saída; depois **redistribui** a saída de volta nas 19 | partições recursivas sobre o arquivo, com área de pivô de 10 registros |
| Custo extra característico | nenhum | uma leitura + uma escrita por registro em cada redistribuição | leituras e escritas posicionais |
| Usa da base | registro, fita, estatisticas, seleção por substituição | os mesmos | registro, estatisticas e um arquivo binário com `fseek` |

O que os métodos 1 e 2 compartilham além da base: a seleção por substituição é o mesmo algoritmo com tamanho de heap diferente (10 e 19). Vale extrair para um módulo `selecao_substituicao.h/.c` que recebe o vetor de fitas de destino, a quantidade e o tamanho da memória, e devolve quantos blocos gerou. Quem terminar a fase 1 primeiro escreve o módulo; o outro reaproveita.

O método 3 não usa `Fita`, porque precisa ler e escrever em posições arbitrárias. Ele pode usar um arquivo binário de `Registro` com `fseek`, com funções próprias de leitura e escrita posicionais que contam em `est`. Para o `main` isso é invisível: no fim o método 3 copia o resultado para a `Fita` de saída em um único bloco, ou o grupo acorda outra forma de devolver o resultado.

Fluxo do método 2 como referência:

1. Fase 1: carrega até 19 registros no vetor, constrói heap de mínimo por nota. Repete: grava a raiz na fita corrente, lê o próximo; se a nota lida é maior ou igual à última gravada entra na raiz e desce, senão fica congelada no fim do vetor e o heap ativo encolhe. Heap ativo vazio fecha o bloco, avança para a próxima fita no rodízio e faz heapify sobre os congelados. Entrada esgotada: esvazia o heap ativo e depois grava os congelados como último bloco.
2. Fase 2, intercalação: rebobina as 19 fitas de entrada e reinicia a saída. Enquanto alguma fita de entrada tem bloco: abre um bloco na saída, carrega o primeiro registro do bloco de cada fita em um heap de até 19 cabeças, retira o menor, grava, repõe da mesma fita até o bloco dela acabar; fecha o bloco e avança cada fita para o próximo bloco.
3. Se a saída tem 1 bloco, fim. Senão, redistribui: rebobina a saída, reinicia as 19 entradas, copia bloco a bloco em rodízio, e volta ao passo 2.

## Fluxo do programa principal (main.c)

O `main` ainda está vazio. O fluxo acordado, em ordem:

1. Valida os argumentos: método entre 1 e 3, quantidade maior que zero, situação entre 1 e 3, `-P` opcional em qualquer posição após os três obrigatórios. Qualquer erro imprime a forma de uso e sai com código diferente de zero.
2. Pré-processamento da situação. Situação 3 usa PROVAO.TXT direto. Situações 1 e 2 exigem um arquivo com os `n` primeiros registros em ordem ascendente ou descendente de nota; esse arquivo é gerado a partir do original por um módulo `preprocessamento` que usa `registroLeTexto` e `registroEscreveTexto` com `est = NULL`. Esse passo não entra nas estatísticas nem no tempo, porque é preparação do experimento, não parte do método.
3. Com `-P`, imprime os `n` registros de entrada com `registroImprime`, sem contar.
4. Abre o arquivo de entrada e a `Fita` de saída, zera `Estatisticas`, liga o relógio, chama `metodoN`, desliga o relógio.
5. Imprime as estatísticas com `estatisticasImprime`. Com `-P`, imprime o resultado com `fitaImprime`, sem contar.
6. Fecha a fita de saída (o que apaga o temporário) e o arquivo de entrada.

O que o `main` nunca faz: ler registro, comparar nota ou tocar em fita de trabalho. Isso pertence aos métodos.

## Convenções do grupo

Contagem. Conta toda transferência de registro entre disco e memória que faça parte do algoritmo: ler PROVAO.TXT ou o arquivo pré-processado, escrever e ler fitas, redistribuir. Não conta: gerar os arquivos ascendente e descendente, imprimir com `-P`, percorrer a fita final para imprimir. Só comparação nota com nota conta, inclusive a comparação com a última nota gravada na seleção por substituição. Comparação de índices, flags ou tamanhos não conta. Toda comparação passa por `comparaNota`; todo I/O passa por registro ou fita.

Tempo. O relógio envolve apenas a chamada do método. Pré-processamento e impressão ficam fora.

Erros. A base trata falha de I/O e uso incorreto das fitas com mensagem em `stderr` e `exit(EXIT_FAILURE)`. O `main` trata argumentos inválidos e arquivo de entrada inexistente. Um `n` maior que o arquivo ordena o que existe e avisa; `n` menor ou igual a zero é erro.

Nomes. Funções com prefixo do módulo em camelCase (`fitaLe`, `registroLeTexto`). Constantes de memória e fitas com nome por método (por exemplo `MEM_METODO2 19`, `FITAS_ENTRADA_METODO2 19`), nunca números soltos no código. Comentários em português sem acentos no código-fonte, para evitar problema de codificação no compilador do avaliador.

Compilação. Um Makefile único gera o executável `ordena` com `-Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L`. Todo arquivo da base deve compilar sem avisos antes de ir para `main`.

## Estado atual e próximos passos

Em 13/11/2026 a base tem os três módulos escritos como rascunho e ainda não compilados nem versionados. Nenhum método foi iniciado.

| Arquivo | Estado |
| --- | --- |
| estatisticas.h / .c | rascunho completo |
| registro.h / .c | rascunho completo |
| fita.h / .c | rascunho completo; o `.c` pode ser reescrito por quem implementar a fase 2 |
| main.c | vazio |
| metodos.h, metodo1.c, metodo2.c, metodo3.c | não criados |
| selecao\_substituicao.h / .c | não criado |
| preprocessamento.h / .c | não criado |
| Makefile | não criado |

Próximos passos da base, em `main`:

- [ ] Compilar os três módulos sem avisos e fazer o primeiro commit da base
- [ ] Fechar o contrato `metodoN` em `metodos.h` com o grupo
- [ ] Escrever `main.c` com validação de argumentos e o fluxo da seção anterior
- [ ] Escrever o pré-processamento das situações 1 e 2
- [ ] Makefile

Fluxo no git: `main` guarda só a base e o `main.c`. Cada método nasce em uma branch (`metodo1`, `metodo2`, `metodo3`) a partir de `main`. Mudança em cabeçalho compartilhado vai para `main` primeiro, com aviso ao grupo, e as branches fazem merge de `main`. Um método entra em `main` quando produz saída ordenada para os seis tamanhos nas três situações e compila sem avisos.

PROVAO.TXT fica fora do repositório (`.gitignore`), assim como os arquivos `fitaNN.tmp` e os arquivos pré-processados, que devem ser adicionados ao `.gitignore` quando existirem.

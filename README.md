<div align="center">
  <h1>🗂️ Ordenação Externa — Análise Experimental de Algoritmos</h1>
  <p>Implementação modular em C e estudo comparativo de desempenho entre métodos de ordenação externa aplicados à base de dados do Provão (471.705 registros).</p>
</div>

<br/>

## 📖 Sobre o Projeto
Desenvolvido para a disciplina de **Estrutura de Dados II (BCC203)** da **Universidade Federal de Ouro Preto (UFOP)**, este projeto implementa e avalia experimentalmente três estratégias clássicas de **Ordenação Externa** sob restrições estritas de memória principal (RAM).

📄 **[Acesse o Relatório Técnico Completo (PDF)](./relatorioTP2.pdf)**.

### Base de Dados (`PROVAO.TXT`)
Os algoritmos ordenam até **471.705 registros** de estudantes em ordem ascendente de nota. Cada registro possui tamanho fixo e contém:
* **Número de inscrição:** inteiro longo (colunas 1–8)
* **Nota:** valor real entre `0.0` e `100.0` — *chave de ordenação* (colunas 10–14)
* **Estado:** cadeia de 2 caracteres (colunas 16–17)
* **Cidade:** cadeia de 50 caracteres (colunas 19–68)
* **Curso:** cadeia de 30 caracteres (colunas 70–99)

---

## ⚙️ Estratégias Implementadas

1. **Intercalação Balanceada de Vários Caminhos ($2f$ fitas):**
   * Geração de blocos iniciais via **seleção por substituição** utilizando um *min-heap* com capacidade máxima para **10 registros**.
   * Utiliza **20 fitas** externas (10 de entrada e 10 de saída), alternando os papéis das fitas a cada passada de intercalação.

2. **Intercalação Balanceada de Vários Caminhos ($f+1$ fitas):**
   * Geração de blocos via **seleção por substituição** com *min-heap* de até **19 registros**.
   * Utiliza **19 fitas de entrada e 1 fita de saída**, realizando a redistribuição circular dos blocos após cada passada.

3. **QuickSort Externo:**
   * Conversão prévia do arquivo texto para arquivo binário temporário e particionamento *in-place* em três regiões (esquerda, pivô central e direita).
   * Área interna limitada a **10 registros** (`LIMITE_MEMORIA`), com chamadas recursivas priorizando sempre a menor partição para otimizar a pilha de execução.

---

## 📊 Resultados Experimentais (471.705 Registros — Ordem Aleatória)

Os experimentos avaliaram volumes de $n \in \{100, 200, 2.000, 20.000, 200.000, 471.705\}$ em entradas crescentes, decrescentes e aleatórias. Abaixo está o resumo comparativo no cenário de maior volume ($n = 471.705$, ordem aleatória):

| Método | Leituras (E/S) | Escritas (E/S) | Comparações | Pré-processamento | Tempo de Ordenação |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **Intercalação $2f$ Fitas** | `2.830.230` | `2.830.230` | `11.488.802` | `6,25 s` | **`26,74 s`** |
| **Intercalação $f+1$ Fitas** | `3.773.640` | `3.773.640` | `8.747.132` | `6,87 s` | `41,77 s` |
| **QuickSort Externo** | `7.933.570` | `7.933.570` | `15.003.262` | `6,84 s` | `115,80 s` |

### Principais Conclusões:
* **Eficiência de E/S:** Os métodos de intercalação superaram o QuickSort Externo em todos os cenários de grande porte devido ao menor custo de movimentação sequencial em memória secundária.
* **$2f$ vs. $f+1$:** A intercalação $2f$ obteve o menor tempo total (`26,74 s`) por dispensar a etapa extra de redistribuição de blocos exigida pelo método $f+1$.
* **Melhor caso:** Em entradas já ordenadas crescentemente, ambos os métodos de intercalação realizaram apenas **1 leitura e 1 escrita por registro** (`471.705` operações), concluindo a ordenação em menos de `0,03 s`.

---

## 🛠️ Ferramentas e Ambiente de Desenvolvimento
* **Linguagem:** C (`C99`)
* **Compilação e Depuração:** GCC e Valgrind (verificação de vazamentos de memória)
* **Ambiente:** Linux / WSL (Windows Subsystem for Linux)

---

## 🚀 Como Compilar e Executar

### 1. Compilação
```bash
# Clone o repositório
git clone [https://github.com/stephanyft/NOME-DO-REPOSITORIO.git](https://github.com/stephanyft/NOME-DO-REPOSITORIO.git)
cd NOME-DO-REPOSITORIO

# Compile os módulos em C
gcc -std=c99 -Wall -Wextra *.c -o ordena

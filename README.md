# Dec_24_C_Virtual-Paging

[🇧🇷 Leia em Português](#português) | [🇺🇸 Read in English](#english)

## Português

**Data:** Dezembro, 2024

**Autores:**

- Guilherme Riechert Senko
- Pedro Barizon

## Simulador de memória virtual

### Resumo do projeto

Este projeto visa implementar e avaliar um simulador de memória virtual, cujo objetivo principal é explorar o desempenho de quatro algoritmos de substituição de páginas em diferentes cenários. Os algoritmos analisados são:

- _Least Recently Used_ (LRU);
- FIFO Segunda Chance (SC);
- _Not Recently Used_ (NRU);
- Algoritmo Ótimo (_Optimal_).

### Arquitetura

O simulador foi arquitetado em três módulos, cada um com uma função específica:

- **`sim_virtual.c`:** Executa a simulação, processando os acessos à memória a partir de um arquivo _log_ de entrada. Este módulo calcula as métricas de número de faltas de página (_page faults_) e de páginas escritas em disco (_page writes_), baseando-se nos _status_ das páginas durante a execução dos algoritmos.
- **`virtual_mem.c`:** Implementa as estruturas de dados essenciais para a simulação. O principal componente é uma lista simplesmente encadeada com ponteiros para o início e para o final, chamada `PageList`, de maneira a permitir fácil manipulação das páginas pelos algoritmos, que podem se utilizar da lista como uma fila com variações.
- **`subs_method.c`:** Define as rotinas específicas para cada algoritmo de substituição de página. Cada algoritmo é composto por três funções principais:
  - `add`: Insere uma nova página na estrutura.
  - `subs`: Remove a página de acordo com a política do algoritmo.
  - `update`: Atualiza os atributos da página quando esta é referenciada quando já está alocada em memória.

### Implementação dos algoritmos

Dada uma nova referência de página, existem três condições possíveis:

1. A página já está em `page_list`.
2. A página não está em `page_list`:
   a. E o número de entradas de `page_list` é menor que `entry_max`.
   b. E o número de entradas de `page_list` é igual a `entry_max`.

#### _Least Recently Used_ (LRU)

O algoritmo LRU substitui a página que não foi utilizada há mais tempo. Define-se uma estrutura de fila, chamada `page_list`, com um certo limite de elementos, chamado `entry_max`. A ideia é que as páginas mais recentemente referenciadas fiquem ao final da fila. Assim, removendo-se do início, retiram-se sempre as páginas menos recentemente utilizadas (_Least Recently Used_). Diante disso:

- Se ocorrer 1:
  Remove a página atual de `page_list`, atualiza seus atributos, e reinsere ao final da fila. Afinal, como a página foi referenciada por último, deve figurar no fim.
- Se ocorrer 2.a:
  Apenas insere a página no fim da fila, pelo mesmo motivo da situação 1.
- Se ocorrer 2.b:
  Remove a página _Least Recenty Used_, isto é, a página no início da fila. Insere a nova página ao final.

#### FIFO Segunda Chance (SC)

Define-se uma estrutura de fila, chamada `page_list`, com um certo limite de elementos, chamado `entry_max`. Cada página possui uma _flag_ chamada `referenced`, que indica se esta foi recentemente referenciada. A ideia é que sejam removidas as páginas mais antigas, isto é, aquelas que tenham entrado na fila primeiro — ficando, portanto, ao início. Há, porém, um detalhe: partindo-se do início da fila, ao se tentar remover uma página, se a _flag_ `referenced` estiver ativa, é dada uma segunda chance, desativando-se a _flag_ da página atual e passando-se para a próxima. Caso todas as páginas sejam percorridas, retorna-se o primeiro elemento da fila, emulando-se uma fila circular. Diante disso:

- Se ocorrer 1:
  Ativa `referenced` da página referenciada, mas não altera sua posição na fila.
- Se ocorrer 2.a:
  Apenas insere a página no fim da fila, com a _flag_ `referenced` ativada.
- Se ocorrer 2.b:
  Aplica o procedimento descrito no parágrafo anterior, removendo a página mais antiga que não tem segunda chance.

#### _Not Recently Used_ (NRU)

Define-se uma estrutura de fila, chamada `page_list`, com um certo limite de elementos, chamado `entry_max`. Cada página possui uma _flag_ chamada `referenced`, que indica se esta foi recentemente referenciada, e outra, chamada `modified`, que indica modificação da página. Define-se a função de prioridade NRU como:

$ p(M, R) = M + 2R $,

em que M é a `modified`; e R, `referenced`. Se estiverem ativadas, possuem valor 1. Do contrário, 0. Isso determina quatro classes de páginas:

| Classe                            | Prioridade |
| --------------------------------- | ---------- |
| Referenciada e modificada         | 3          |
| Referenciada e não modificada     | 2          |
| Não referenciada e modificada     | 1          |
| Não referenciada e não modificada | 0          |

A ideia é que as páginas de `page_list` sejam ordenadas crescentemente segundo a função de prioridade $p$, de modo que as de menor prioridade fiquem ao início da fila. Em caso de empate, arbitra-se a adoção do critério LRU: a mais recentemente utilizada terá maior prioridade. Tal escolha é baseada no **Princípio da Localidade Temporal**.

Além disso, define-se uma rotina de desativação das _flags_ de referência, a fim de trazer dinamismo à fila. Assim, arbitra-se que, após `entry_max` novas referências, todas as _flags_ referenced são desativadas. Diante disso:

- Se ocorrer 1:
  Remove página atual da fila e atualiza seus atributos. Incrementa o contador de referências. Se necessário, executa a rotina de desativação de _flags_ `referenced`, reordena a fila e reinicia o contador. Reinsere de forma ordenada a página atual.
- Se ocorrer 2.a:
  Incrementa o contador de referências. Se necessário, executa a rotina de desativação de _flags_ `referenced`, reordena a fila e reinicia o contador. Insere de forma ordenada a página atual.
- Se ocorrer 2.b:
  Incrementa o contador de referências. Se necessário, executa a rotina de desativação de _flags_ `referenced`, reordena a fila e reinicia o contador. Remove o primeiro elemento da fila. Insere de forma ordenada a nova página.

#### Algoritmo Ótimo (_Optimal_)

Remove a página referenciada no futuro mais distante.

Define-se uma estrutura de fila, chamada `page_list`, com um certo limite de elementos, chamado `entry_max`. Cada página possui uma entrada chamada `next_ref`, que indica a próxima linha em que a página é referenciada no arquivo `file` de endereços virtuais. Caso não haja mais referências, define-se a constante simbólica `PINF` (_Page Index Not Found_) como o maior valor representável por um inteiro sem sinal, que se supõe maior que o número de linhas de file. A ideia é que, toda vez que uma página for referenciada, `file` continue sendo percorrido até que encontre a próxima linha de referência, retornando, em seguida, à posição original.

Assim, ordenam-se as páginas decrescentemente pelo número da linha da próxima referência. Dessa forma, as páginas referenciadas no futuro mais distante ficam ao início, de modo que a substituição se limite à remoção do primeiro elemento. Caso uma página já na fila seja referenciada, basta removê-la, atualizar sua `next_ref` e reinseri-la ordenadamente.

Por fim, vale mencionar que, caso duas ou mais páginas não possuam mais referência, o algoritmo removerá primeiro as que não tiverem sido modificadas, para reduzir o número de escritas em disco. Diante disso:

- Se ocorrer 1:
  Remove a página, atualiza sua `next_ref` e seus outros atributos, e reinsere ordenadamente.
- Se ocorrer 2.a:
  Obtida a `next_ref`, apenas insere a página ordenadamente.
- Se ocorrer 2.b:
  Remove a primeira página da fila e insere a nova página ordenadamente.

### Testes

Os testes foram realizados com quatro arquivos `.log` de entrada simulando diferentes padrões de acesso. Além disso, variaram-se o tamanho das páginas (ora 8 kB, ora 32 kB) e o da memória princial (1 MB, ora 2 MB). Foi gerado um total de 32 [gráficos](https://docs.google.com/spreadsheets/d/1HnZrXXxIdN1HFNX85y-cW_JEE9XiMo_xyHprkrHulz0/edit?usp=sharing).

### Observações e conclusões

A partir dos gráficos, observa-se que o aumento no tamanho das páginas tende a aumentar o número de _page faults_, uma vez que se reduz o número máximo de páginas em memória. Por raciocínio análogo, percebe-se que um aumento no tamanho da memória principal mantendo fixo o tamanho das páginas tende a reduzir o número de faltas de página.

Quanto ao desempenho dos algoritmos, verifica-se que, obviamente, o Ótimo sempre resultou no menor número de faltas de páginas, mas, surpreendentemente, também no menor número de páginas escritas. Este último fato se deve à otimização implementada quanto à substituição de páginas sujas. Em seguida, o LRU parece ter gerado o segundo menor número de _page faults_, com o NRU logo atrás. Quanto à escrita de páginas, a situação se inverte entre os dois. Afinal, o NRU possui otimizações que objetivam evitar a escrita de páginas modificadas. Por outro lado, vale mencionar que se ter usado o critério LRU como desempatador do NRU fez com que as faltas de páginas de ambos os algoritmos fossem próximas em todas as situações. Por fim, ve-se que o FIFO Segunda Chance apresentou os piores resultados em todos os quesitos, o que aponta para a ineficácia da política FIFO.

Em face do exposto, para estudos futuros, sugere-se explorar algoritmos híbridos ou adaptativos que combinem os pontos fortes das abordagens analisadas.

## English

**Date:** December, 2024

**Authors:**

- Guilherme Riechert Senko
- Pedro Barizon

## Virtual Memory Simulator

### Project Summary

This project aims to implement and evaluate a virtual memory simulator, with the main objective of exploring the performance of four page replacement algorithms in different scenarios. The algorithms analyzed are:

- Least Recently Used (LRU);
- FIFO Second Chance (SC);
- Not Recently Used (NRU);
- Optimal Algorithm (Optimal).

### Architecture

The simulator is structured in three modules, each with a specific function:

- **`sim_virtual.c`:** Runs the simulation, processing memory accesses from an input log file. This module calculates metrics such as number of page faults and pages written, based on the status of pages during the execution of the algorithms.
- **`virtual_mem.c`:** Implements the essential data structures for the simulation. The main component is a singly linked list with pointers to the beginning and end, called `PageList`, which allows easy manipulation of pages by the algorithms, enabling them to treat the list as a queue with variations.
- **`subs_method.c`:** Defines the routines specific to each page replacement algorithm. Each algorithm consists of three main functions:
  - `add`: Inserts a new page into the structure.
  - `subs`: Removes the page according to the algorithm's policy.
  - `update`: Updates the page's attributes when it is referenced while already allocated in memory.

### Algorithm Implementation

Given a new page reference, there are three possible conditions:

1. The page is already in `page_list`.
2. The page is not in `page_list`:
   a. And the number of entries in `page_list` is less than `entry_max`.
   b. And the number of entries in `page_list` equals `entry_max`.

#### Least Recently Used (LRU)

The LRU algorithm replaces the page that has not been used for the longest time. A queue structure, called `page_list`, is defined with a certain limit of elements, called `entry_max`. The idea is that the most recently referenced pages are at the end of the queue. Thus, removing from the front always removes the least recently used pages. In this case:

- If condition 1 occurs:
  Removes the current page from `page_list`, updates its attributes, and reinserts it at the end of the queue. Since the page was referenced last, it should be at the end.
- If condition 2.a occurs:
  Simply inserts the page at the end of the queue, for the same reason as condition 1.
- If condition 2.b occurs:
  Removes the Least Recently Used page, which is the page at the front of the queue. Inserts the new page at the end.

#### FIFO Second Chance (SC)

A queue structure, called `page_list`, is defined with a certain limit of elements, called `entry_max`. Each page has a flag called `referenced`, which indicates whether it has been recently referenced. The idea is to remove the oldest pages, i.e., those that entered the queue first — therefore, they are at the front. However, there is a detail: starting from the front of the queue, when attempting to remove a page, if the `referenced` flag is active, it gets a second chance, the flag of the current page is deactivated, and the process moves to the next page. If all pages are traversed, the first element in the queue is returned, emulating a circular queue. In this case:

- If condition 1 occurs:
  Activates the `referenced` flag of the referenced page but does not change its position in the queue.
- If condition 2.a occurs:
  Simply inserts the page at the end of the queue, with the `referenced` flag activated.
- If condition 2.b occurs:
  Applies the procedure described in the previous paragraph, removing the oldest page that does not have a second chance.

#### Not Recently Used (NRU)

A queue structure, called `page_list`, is defined with a certain limit of elements, called `entry_max`. Each page has a flag called `referenced`, which indicates whether it has been recently referenced, and another called `modified`, which indicates if the page was modified. The NRU priority function is defined as:

$ p(M, R) = M + 2R $,

where M is `modified`, and R is `referenced`. If both are active, they have a value of 1. Otherwise, they have a value of 0. This determines four classes of pages:

| Class                           | Priority |
| ------------------------------- | -------- |
| Referenced and modified         | 3        |
| Referenced and not modified     | 2        |
| Not referenced and modified     | 1        |
| Not referenced and not modified | 0        |

The idea is to sort the pages in `page_list` in increasing order according to the priority function $p$, so that the lowest priority pages are at the front of the queue. In case of a tie, the LRU criterion is used: the most recently used page has higher priority. This choice is based on the **Principle of Temporal Locality**.

Additionally, a routine to deactivate the reference flags is defined to bring dynamism to the queue. It is assumed that after `entry_max` new references, all the `referenced` flags are deactivated. In this case:

- If condition 1 occurs:
  Removes the current page from the queue and updates its attributes. Increments the reference counter. If necessary, executes the routine to deactivate the `referenced` flags, reorders the queue, and resets the counter. Re-inserts the current page in sorted order.
- If condition 2.a occurs:
  Increments the reference counter. If necessary, executes the routine to deactivate the `referenced` flags, reorders the queue, and resets the counter. Inserts the current page in sorted order.
- If condition 2.b occurs:
  Increments the reference counter. If necessary, executes the routine to deactivate the `referenced` flags, reorders the queue, and resets the counter. Removes the first element in the queue. Inserts the new page in sorted order.

#### Optimal Algorithm (Optimal)

Removes the page that will be referenced the furthest in the future. A queue structure, called `page_list`, is defined with a certain limit of elements, called `entry_max`. Each page has an entry called `next_ref`, which indicates the next line in which the page is referenced in the virtual address `file`. If there are no further references, the symbolic constant `PINF` (Page Index Not Found) is defined as the largest value representable by an unsigned integer, assumed to be larger than the number of lines in the file. The idea is that each time a page is referenced, the file is traversed until the next reference line is found, then it returns to the original position.

Thus, the pages are sorted in descending order by the line number of the next reference. In this way, pages referenced the furthest in the future are at the front, and the replacement is limited to removing the first element. If a page already in the queue is referenced, it is simply removed, its `next_ref` is updated, and it is reintegrated in sorted order.

Finally, it is worth mentioning that if two or more pages have no further references, the algorithm will first remove those that have not been modified, to reduce the number of writes to disk. In this case:

- If condition 1 occurs:
  Removes the page, updates its `next_ref` and other attributes, and reinserts it in sorted order.
- If condition 2.a occurs:
  Obtains the `next_ref` and simply inserts the page in sorted order.
- If condition 2.b occurs:
  Removes the first page from the queue and inserts the new page in sorted order.

### Tests

The tests were performed with four input `.log` files simulating different access patterns. Additionally, the page sizes (either 8 KB or 32 KB) and the main memory sizes (1 MB or 2 MB) were varied. A total of 32 [graphs](https://docs.google.com/spreadsheets/d/1HnZrXXxIdN1HFNX85y-cW_JEE9XiMo_xyHprkrHulz0/edit?usp=sharing) were generated.

### Observations and Conclusions

From the graphs, it can be observed that increasing the page size tends to increase the number of page faults, as the maximum number of pages in memory is reduced. By similar reasoning, increasing the size of the main memory while keeping the page size fixed tends to reduce the number of page faults.

Regarding the performance of the algorithms, it is clear that, as expected, the Optimal algorithm always resulted in the lowest number of page faults but, surprisingly, also in the fewest number of page writes. This last fact is due to the optimization implemented for dirty page replacement. Next, LRU seems to have generated the second lowest number of page faults, with NRU right behind it. Regarding page writes, the situation reverses between the two. After all, NRU has optimizations aimed at avoiding modified pages to be written. On the other hand, it is worth mentioning that using the LRU criterion as a tie-breaker for NRU caused the page fault rates of both algorithms to be similar in all situations. Finally, it can be seen that FIFO Second Chance produced the worst results in all aspects, indicating the ineffectiveness of the FIFO policy.

In light of the above, for future studies, it is suggested to explore hybrid or adaptive algorithms that combine the strengths of the approaches analyzed.

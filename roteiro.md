# MC723 - Projeto 2

## Integrantes:

| Nome|RA|
|---|---|
|André Nogueira Brandão |116130|
|Matheus Pinheiro dos Santos |119920|
|Renan Camargo de Castro| 147775|
|Gustavo Rodrigues Basso| 105046|

## Roteiro:

Avaliaremos o impacto, no desempenho de um processador, de várias características arquiteturais estudadas na disciplina teórica.

Escolhemos os seguintes benchmarks para serem avaliados:

- **Qsort**: por ser um programa simples, pequeno e fácil de ser modificado
- **Lame**: usa uma boa capacidade de processamento e memória
- **Sha**: usa muitas expressões aritméticas
- **FFt**: uso intenso de processador

### Estratégia

**Tamanho do pipeline**:

- Primeiro mediremos os ciclos sem pipeline
- Como o simulador MIPS não fornece uma implementação de pipeline, modificaremos o código para contar os ciclos supondo a existência de pipeline de 5, 7 e 13 estágios

**Processador escalar vs superescalar**:

- Definido um tamanho de pipeline, vamos manter o histórico das instruções e analisar as novas instruções 2 a 2
- Caso haja dependẽncia de alguma delas com alguma instrução que está no "pipeline", contabilizamos a quantidade de ciclos de stall necessários para resolver o problema de acordo com o tamanho do pipeline

**Hazard de dados e controle**:

- Para a contagem dos Hazard utilizaremos a mesma estratégia do pipeline. Vamos utilizar um pipeline de 5 estágios.
- Faremos mudanças nas instruções do simulador para armazenar as instruções executadas e avaliar quando um hazard acontece.

Vamos avaliar os seguintes hazards de dados(considerando i1 uma instrução que ocorre antes de i2 na):

* read after write (RAW), nesse caso, a instrução i2 tenta acessar uma informação escrita por i1. Existe uma dependência entra a saída de i1 e a entrada de i2. Ex: (i1,i2) - **R2** <- R1 + R3, R4 <- **R2** + R3. - *Será analisada para os tipos **escalar/superescalar**.*
* write after read (WAR), nesse caso, i2 tenta escrever em um local, antes de ser lido por i1. Ex: (i1,i2) - R4 <- R1 + **R5**, **R5** <- R1 + R2. - *Será analisada para o tipo **superescalar**, pois só acontece com concorrência.*
* write after write (WAW), i2 tenta escrever no mesmo lugar de i1. Ex: (i1,i2) - **R2** <- R4 + R7, **R2** <- R1 + R3. - *Será analisada para o tipo **superescalar**, pois só acontece com concorrência.*

Para os tipos que podem ser resolvidos com forwarding, vamos considerá-los resolvidos.


E para os hazards de controle, iremos analisar o atraso que uma instrução de **branch** tem no pipeline.


**Branch Predictor**:

Iremos fixar os parâmetros passados e adotaremos duas estratégias de branch Predictor

1. Assumiremos que o desvio sempre ocorrerá
2. O desvio nunca ocorrerá
3. Sem implementação alguma de Branch Predictor


Para ambos os casos vamos manter um histórico das instruções e sempre que fomos executar uma instrução de branch, vamos verificar qual a próxima instrução que o simulador vai rodar e se ela está de acordo com a predição. Caso não esteja, iremos contabilizar os stalls.


**Cache**:

Iremos salvar um trace da execução do simulador para poder avaliar no software Dinero.

Vamos avaliar as melhores 4 configurações para cada benchmark se baseando no laboratório anterior:

* L1 - Size: **128K**, Block Size: 128B; L2 - Size: **1024K**, Block Size: 128B.
* L1 - Size: **64K**, Block Size: 128B; L2 - Size: **2048K**, Block Size: 128B.
* L1 - Size: **128K**, Block Size: 128B; L2 - Size: **2048K**, Block Size: 128B.
* L1 - Size: **64K**, Block Size: 128B; L2 - Size: **1024K**, Block Size: 128B.


### Planejamento


1. Implementar o pipeline e o processador superescalar - até dia 06/05
2. Hazard e Branch predictor - até dia 13/05
3. Cache e finalização do relatório - até dia 19/05  

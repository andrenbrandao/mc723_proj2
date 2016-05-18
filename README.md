# MC723 - Projeto 2 - Grupo 2
## Integrantes:

| Nome|RA|
|---|---|
|André Nogueira Brandão |116130|
|Matheus Pinheiro dos Santos |119920|
|Renan Camargo de Castro| 147775|
|Gustavo Rodrigues Basso| 105046|

## Configurações Arquiqueturais:

 - Tamanho do pipeline: 5 estágios
 - Cache:
    - As configurações de cache foram escolhidas com base nos resultados coletados no exercicio de numero 2. As melhores configurações foram:
    - **C1**: L1 - Size: 128K, Block Size: 128B; L2 - Size: 1024K, Block Size: 128B.
    - **C2**: L1 - Size: 64K, Block Size: 128B; L2 - Size: 2048K, Block Size: 128B.
    - **C3**: L1 - Size: 128K, Block Size: 128B; L2 - Size: 2048K, Block Size: 128B.
    - **C4**: L1 - Size: 64K, Block Size: 128B; L2 - Size: 1024K, Block Size: 128B.

## Medidas de Desempenho:
Medimos o impacto, no desempenho de um processador, de diferentes características através das seguintes métricas:
 - **Tamanho do Pipeline:** Calculo do número de ciclos executados para diferentes tamanhos de pipeline (sem pipeline, 5 estagios, 7 estagio e 13 estagios)
 - **Processador escalar vs. Superescalar:** 
    - Contagem do número total de ciclos executados em cada processador;
    - Contagem do números de ciclos de stalls para cada processador.
 - **Hazard de Dados e Controle:**
    - Identificação dos estagios em que ocorrem Hazards; 
    - Hazard de Dados: Contagem do número de stalls quando um hazard de controle ocorre (brench predictor falha) para os seguintes casos:
        - Read after Write - analise de processador escalar vs superescalar
        - write after read - analise apenas de processador superescalar, pois ocorre apenas quando existe concorrência
        - Write after write - analise apenas para superescalar, pois ocorre apenas quando existe concorrência
    - Hazard de Controle: Será analisado no ponto seguinte, de Branch de Controle.
 - **Branch Predictor:** Contagem do número de ciclos de stalls gerados quando o branch não segue a previsão. Sera analisado para cada uma das seguintes estratégias de branch predictor:
    - Always Taken: o desvio é sempre tomado
    - Never Taken: o desvio nunca é tomado
    - Sem Branch Predictor
 - **Cache:** Analise do trace gerado pelo Dinero para as configurações C1, C2, C3 e C4 de cache, descritas na sessão anterior.
 
 Para realizar as medidas foram necessarios fixar parâmetros. OS parametros que foram fixados estarão decritos em cada seção.
## Resultados Obtidos:
### Tamanho do Pipeline

| |**Sem Pipeline**|**5 estágios**|**7 estágios**|**13 estágios**|
|---|---|---|---|---|
|**QSort** |||||
|**Lame** |||||
|**Sha**|||||
|**FFT**|||||

- Analise:

### Processadorescalar vs Superescalar:
- Parâmetros Fixos:
    - Tamanho da pipeline: 5 estágios
    - Número de instruções por ciclo de clock do processador superescalar: 2.
- Contagem do número total de ciclos executados em cada processador:

| |**Processador Escalar**|**Processador Superescalar**|
|---|---|---|
|**QSort** |||
|**Lame** |||
|**Sha**|||
|**FFT**|||

- Contagem do números de ciclos de stalls para cada processador:

| |**Processador Escalar**|**Processador Superescalar**|
|---|---|---|
|**QSort** |||
|**Lame** |||
|**Sha**|||
|**FFT**|||

- Analise: Como o processador escalar explora o paralelismo em nivel de instrução, para um programa que executa n instruções temos que o resultado, teorico, esperado para o número total de ciclos executados é:
    - Processador escalar: n + 5 + # de ciclos de stall 
    - Processador supesescalar: n/2 + 5 + # de ciclos de stall

### Hazard de Dados e Controle
- Parâmetros Fixos:
    - Tamanho da pipeline: 5 estágios. 
- Identificação dos estagios em que ocorrem Hazards:
    - Hazards acontecem somente em processadores escalares, no estagio memoria/registrador. Os demais casos de hazard identificados podem ser resolvidos através de *pipeline fowarding*.
- Contagem do número de stalls quando um hazard de controle ocorre (brench predictor falha).
    -  Read after Write

        | |**Processador Escalar**|**Processador Superescalar**|
        |---|---|---|
        |**QSort** |||
        |**Lame** |||
        |**Sha**|||
        |**FFT**|||

    - Write after Read
    
        | |**Processador Superescalar**|
        |---|---|
        |**QSort** ||
        |**Lame** ||
        |**Sha**||
        |**FFT**||

    - Write after Write
    
        | |**Processador Superescalar**|
        |---|---|
        |**QSort** ||
        |**Lame** ||
        |**Sha**||
        |**FFT**||

- Analise:

### Branch Predictor:
- Parâmetros Fixos:
    - Tamanho da pipeline: 5 estágios;
    - Processador Escalar;
    - Sem Hazard de Dados.
 - Contagem do número de ciclos de stalls gerados quando o branch não segue a previsão:
 
    | |**Always Taken**|**Never Taken**|**Sem Branch Predictor**|
    |---|---|---|---|
    |**QSort** ||||
    |**Lame** ||||
    |**Sha**||||
    |**FFT**||||

- Analise: 

### Cache:
- Parâmetros Fixos:
    - Tamanho da pipeline: 5 estágios. 
- C1:
- C2:
- C3:
- C4:

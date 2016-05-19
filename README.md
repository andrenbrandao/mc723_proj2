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
    - Hazard de Dados: 
        - Read after Write - analise de processador escalar vs superescalar
        - write after read - analise apenas de processador superescalar, pois ocorre apenas quando existe concorrência
        - Write after write - analise apenas para superescalar, pois ocorre apenas quando existe concorrência
    - Hazard de Controle: Será analisado no ponto seguinte, de Branch de Controle.
 - **Branch Predictor:** Contagem do número de ciclos de stalls gerados quando o branch não segue a previsão. Foi analisado para cada uma das seguintes estratégias de branch predictor:
    - BTFNT: branchs com saltos para trás são preditas como tomadas(taken) e para frente como não tomadas(not taken).
    - Always Not Taken: o desvio nunca é tomado.
    - Sem Branch Predictor.
 - **Cache:** Analise do trace gerado pelo Dinero para as configurações C1, C2, C3 e C4 de cache, descritas na sessão anterior.
 
 Para realizar as medidas foram necessarios fixar parâmetros. OS parametros que foram fixados estarão decritos em cada seção.
## Resultados Obtidos:
### Tamanho do Pipeline
- Parâmetros Fixos:
    - Superescalar: não
    - Branch Predictor: Sem branch predictor

| |**Sem Pipeline**|**5 estágios**|**7 estágios**|**13 estágios**|
|---|---|---|---|---|
|**QSort** ||36621328|||
|**Susan** ||398309935|||
|**Sha**||142004727|||
|**FFT**||580578586|||

- Analise:

### Processadorescalar vs Superescalar:
- Parâmetros Fixos:
    - Tamanho da pipeline: 5 estágios
    - Número de instruções por ciclo de clock do processador superescalar: 2.
    - Branch Predicor: sem branch predictor
- Contagem do número total de ciclos executados em cada processador:

| |**Processador Escalar**|**Processador Superescalar**|
|---|---|---|
|**QSort** |36621328|41295537|
|**Susan** |398309935|380371486|
|**Sha**|142004727|107299583|
|**FFT**|580578586|535837659|

- Contagem do números de ciclos de stalls para cada processador:

| |**Processador Escalar**|**Processador Superescalar**|
|---|---|---|
|**QSort** |7285889|26627820|
|**Susan** |53239544|207836288|
|**Sha**|9222862|40908648|
|**FFT**|76479029|283787883|

- Analise: Como o processador escalar explora o paralelismo em nivel de instrução, para um programa que executa n instruções temos que o resultado, teorico, esperado para o número total de ciclos executados é:
    - Processador escalar: n + 5 + # de ciclos de stall 
    - Processador supesescalar: n/2 + 5 + # de ciclos de stall

### Hazard de Dados e Controle
- Parâmetros Fixos:
    - Tamanho da pipeline: 5 estágios. 
- Identificação dos estagios em que ocorrem Hazards:
    - Hazards acontecem somente no estagio memoria/registrador(no tipo raw). Os demais casos de hazard identificados podem ser resolvidos através de *pipeline fowarding*.

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
 - Contagem do número de ciclos de stalls gerados quando o branch não segue a previsão:
 
    | |**BTFNT**|**Always Not Taken**|**Sem Branch Predictor**|
    |---|---|---|---|
    |**QSort** |797948|2021383|2705414|
    |**Susan** |453558|25493628|27816383|
    |**Sha**|34439|5467427|5897443|
    |**FFT**|15131438|21344620|60440260|

- Para contar os stalls quando não há branch Predictor, supomos que os branchs são tratados no segundo estágio da pipeline, logo, adicionamos um ciclo de stall.

- Analise: 

### Cache:
- Parâmetros Fixos:
    - Tamanho da pipeline: 5 estágios. 
- C1:
    - L1:

        | |Total Fetchs|% Read| % Write| % Misses (do total)| % Read miss | %Write Miss|
        |---|---|---|---|---|---|---|
        |QSort |15797640|61.20%|38.80%|0.48%|0.57%|0.33%|
        |Susan |77366303|98.60%|1.40%|0.10%|0.10%|0.28%|
        |Sha|26818003|0.6822|31.78%|2.59%|3.77%|0.04%|
        |FFT|129865348|52.62%|47.38%|0.18%|0.34%|0.01%|    

    - L2
    
        | |Total Fetchs|% Read| % Write| % Misses (do total)| % Read miss | %Write Miss|
        |---|---|---|---|---|---|---|
        |QSort |131167|57.83%|42.17%|15.18%|23.78%|3.39|
        |Susan |79180|96.07%|3.93%|4.02%|4.18%|0%|
        |Sha|697597|99.43%|0.57%|62.22%|62.48%|16.24%|
        |FFT|244602|97.72%|2.28%|60.24%|61.53%|4.47%|
- C2:
    - L1:

        | |Total Fetchs|% Read| % Write| % Misses (do total)| % Read miss | %Write Miss|
        |---|---|---|---|---|---|---|
        |QSort |15797640|61.20%|38.80%|0.57%|0.71%|0.36%|
        |Susan |77366303|98.60%|01.40%|0.26%|0.25%|0.39%|
        |Sha|26818003|68.22%|31.78%|2.78%|4.06%|0.04%|
        |FFT|129865348|52.62%|47.38%|0.20%|0.36%|0.02%|    

    - L2
    
        | |Total Fetchs|% Read| % Write| % Misses (do total)| % Read miss | %Write Miss|
        |---|---|---|---|---|---|---|
        |QSort |158793|56.91%|43.09%|6.32%|11.11%|0%|
        |Susan |202862|97.86%|2.14%|1.57%|1.60%|0%|
        |Sha|749613|99.46%|0.54%|49.60%|49.79%|14.89%|
        |FFT|269084|95.68%|4.32%|52.36%|54.67%|1.06%|
- C3:
    - L1:
    
        | |Total Fetchs|% Read| % Write| % Misses (do total)| % Read miss | %Write Miss|
        |---|---|---|---|---|---|---|
        |QSort |15797640|61.20%|38.80%|0.48%|0.57%|0.33%|
        |Susan |77366303| 98.60%|1.40%|0.10%|0.10%|0.28%|
        |Sha|26818003|68.22%|31.78%|2.59%|3.77%|0.04%|
        |FFT|129865348|52.62%|47.38%|0.18%|0.34%|0.01%|   

    - L2
    
        | |Total Fetchs|% Read| % Write| % Misses (do total)| % Read miss | %Write Miss|
        |---|---|---|---|---|---|---|
        |QSort |131167|57.83%|42.17%|7.65%|13.24%|0%|
        |Susan |79180|96.07%|3.93%|4.02%|4.18%|0%|
        |Sha|697597|99.43%|0.57%|53.30%|53.52%|16.24%|
        |FFT|244602|97.72%|2.28%|57.60%|58.89%|2.44%|
- C4:

    - L1:

        | |Total Fetchs|% Read| % Write| % Misses (do total)| % Read miss | %Write Miss|
        |---|---|---|---|---|---|---|
        |QSort |15797640|61.20%|38.80%|0.57%|0.71%|0.36%|
        |Susan |77366303|98.60%|1.40%|0.26%|0.25%|0.39%|
        |Sha|26818003|68.22%|31.78%|2.78%|4.06%|0.04%|
        |FFT|129865348|52.62%|47.38%|0.20%|0.36%|0.02%|   

    - L2
    
        | |Total Fetchs|% Read| % Write| % Misses (do total)| % Read miss | %Write Miss|
        |---|---|---|---|---|---|---|
        |QSort |158793|56.91%|43.09%|12.10%|20.23%|1.37%|
        |Susan |202862|97.86%|2.14%|1.57%|1.60%|0%|
        |Sha|749613|99.46%|0.54%|57.90%|58.13%|14.89%|
        |FFT|269084|95.68%|4.32%|54.75%|57.13%|1.94%|

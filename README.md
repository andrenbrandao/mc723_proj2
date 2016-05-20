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
    - Identificação dos estágios em que ocorrem Hazards;
    - Hazard de Dados:
        - Read After Write - analise de processador escalar vs superescalar
        - Write After Read - analise apenas de processador superescalar, pois ocorre apenas quando existe concorrência
        - Write After Write - analise apenas para superescalar, pois ocorre apenas quando existe concorrência
    - Hazard de Controle: Será analisado no ponto seguinte, de Branch de Controle.

 - **Branch Predictor:** Contagem do número de ciclos de stalls gerados quando o branch não segue a previsão. Foi analisado para cada uma das seguintes estratégias de branch predictor:
    - BTFNT: branchs com saltos para trás são preditas como tomadas(taken) e para frente como não tomadas(not taken).
    - Always Not Taken: o desvio nunca é tomado.
    - Sem Branch Predictor.

 - **Cache:** Analise do trace gerado pelo Dinero para as configurações C1, C2, C3 e C4 de cache, descritas na sessão anterior. Os dados que coletamos para um dos caches são:
    - Número total de Fetchs
    - % de Reads (do número total de Fetchs)
    - % de Writes (do número total de Fetchs)
    - % de miss
    - % de Read miss
    - % de Write miss

 Para realizar as medidas foram necessarios fixar parâmetros. Os parâmetros que foram fixados estão decritos em cada seção.
## Resultados Obtidos:
### Tamanho do Pipeline
- Parâmetros Fixos:
    - Superescalar: não
    - Branch Predictor: Sem branch predictor
    - **Desconsiderando** hazards (seja de dados ou de controle)

    | | **Sem Pipeline** | **5 estágios** | **7 estágios** | **13 estágios** |
    |---|---|---|---|---|
    | **QSort** |189233668|39454195|39454197|39454203|
    |  **Susan** |1148441221|395746858|395746860|395746866|
    | **Sha** |391062479|137085619|137085621|137085627|
    | **FFT** |1878273331|539993328|539993330|539993336|

*ps: para o caso sem pipeline, consideramos os CPIS médios do laboratório 3.*

*ps2: como foi executado em ocasiões diferentes(por pessoas diferentes), não trata do mesmo número de instruções da próxima seção*

- **Análise:**

Vemos claramente que entre os processadores com pipeline, não há muito diferença se não considerarmos os hazards. Já para a versão com pipeline para a sem pipeline, temos uma média de melhora de aproximadamente **70%** na performance dos aplicativos.

### Processadorescalar vs Superescalar:
- Parâmetros Fixos:
    - Tamanho da pipeline: 5 estágios
    - Número de instruções por ciclo de clock do processador superescalar: 2.
    - Branch Predicor: sem branch predictor

- Neste caso levamos em consideração os hazards de dados. Para o processador superescalar, foram considerados os hazards do tipo:
  - **RAW** - Read after Write
  - **WAR** - Write after Read
  - **WAW** - Write after Write

- Para o processador escalar, apenas o **RAW**.
- Para os hazards na mesma pipeline, apenas foram considerados os casos memória/registrador. Os demais casos de hazard identificados podem ser resolvidos através de **pipeline fowarding**.
- Neste caso, foi considerado também os hazards de controle e consideramos que eles eram resolvidos no segundo estágio da pipeline, ocasionando em uma bolha de 1 ciclo de execução.

- Contagem do número total de ciclos executados em cada processador:

| |**Processador Escalar**|**Processador Superescalar**|
|---|---|---|
|**QSort** |36621328|41295537|
|**Susan** |398309935|380371486| 
|**Sha**|142004727|107299583|
|**FFT**|580578586|535837659|

Como o processador escalar explora o paralelismo em nivel de instrução, para um programa que executa n instruções temos que o resultado, teórico, esperado para o número total de ciclos executados é:

- Processador escalar: n + 5 + # de ciclos de stall
- Processador supesescalar: n/2 + 5 + # de ciclos de stall

Dessa forma, se os números de stalls não variassem muito de um caso para outro o processador superescalar deveria ter um desempenho muito superior ao escalar. Contudo, olhando os dados da tabela, vemos que não é isso o que acontece, pelo contrário, na maioria dos casos (Susan, Sha, Fft) houve uma piora no desempenho e somento no caso do Qsort há uma melhora de 12% no desempenho. Para entender melhor este acontecimento devemos olhar também para o número de stalls em cada caso.

- Contagem do números de ciclos de stalls para cada processador:

| |**Processador Escalar**|**Processador Superescalar**|
|---|---|---|
|**QSort** |7285889|26627820|
|**Susan** |53239544|207836288|
|**Sha**|9222862|40908648|
|**FFT**|76479029|283787883|

Olhando estes dados vemos que o número de stalls aumentou significantemente no caso do processador superescalar, em média de 25% a mais de no caso de um processador escalar. Se analisarmos, para o processador superescalar, o número de ciclos de stalls em relação ao número total de ciclos (tabela anterior) vemos que em torno de 50% do total de ciclos é em razão de stalls resultantes de hazards de dados e de controle.

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

- Para contar os stalls quando não há branch Predictor, supomos que os branchs são tratados no segundo estágio da pipeline, logo, sempre adicionamos um ciclo de stall.

- Análise:
Na média, obtemos uma melhora de aproximadamente para os casos:
  - Always Not Taken:  Melhora de **26,40%**.
  - BTFNT: Melhora de **71,80%**.

Vemos claramente que a estratégia de branch dinâmica funciona muito melhor que a estratégia simples, mas a simples também funciona bem em alguns casos, chegando até 65% de melhora no caso do fft.

Analisando cada um dos programas vemos que: tanto o Sha quanto o Susan obtiveram uma melhora mínima, em torno de 10%, utilizando o *Always Not Taken* mas chegaram há quase 99% na melhora do desempenho no caso do *BTFNT*. Isso provavelmente é explicado por um alto indice de loops durante a execução desses programas, o que faz com que a *BTFNT* se comporte melhor, já que ela se benificia desses casos (loops são resultados de vários jumps para trás (backwards) que são tratados como *taken* nesta predição). O Qsort teve uma melhor significativa com o uso do *Always Not Taken*, em torno de 25%, mas teve um resultado também muito bom com o *BTFNT*, uma melhora de 70%. O FFT apresentou também um comportamento diferente, com uma melhora muito boa já com a utilização do *Always Not Taken*, 65%, e um resultado próximo com o *BTFNT*, em torno de 75%.

Dessa forma, vemos que a *BTFNT* foi a que obteve um melhor desempenho entre elas, chegando a melhora de até impressinantes 99,42%, no caso do Sha, em relação a execução sem branch predictor. 

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

- **Analise**: Dos resultados obtidos é possível perceber que, em geral, alterações no tamanho da cache 1, influenciam o número de fetchs e % de misses da cache 2, da seguinte maneira:
    - Aumento da cache 1: causa diminuição no número de fetchs e aumento na % de misses da cache 2.
    - Diminuição da cache 1: causa aumento no número de fetchs e diminuição na % de misses da cache 2

    O numero de fetchs e a % de misses apresentam esta relação inversa, pois um miss possui maior relevancia quando o número de fetchs é menor.

    Também podemos notar que, em geral, alterações na cache 1 apresentam bem mais impacto na perfomance de um processador do que alterações na cache 2, pois a cache 1 faz um número de fetchs muito maior, o que implica em:
    - diminuição de centesimos na % de misses do cache 1, pode economizar muito mais tempo de processamento do que um aumento de alguns % no miss do cache 2 causaria.

    Vale ressaltar que alterações na cache 2 não surtem efeito nas taxas de miss da cache 1, pois um fetch só é realizado na cache 2, após se realizado na cache 1.

- **Configuração escolhida**: C3: L1 - Size: 128K, Block Size: 128B; L2 - Size: 2048K, Block Size: 128B.
    - Assim como no exercício 2, as configurações com o maior tamanho de cache, em geral, apresentam um resultado melhor. Porém é interessante ressaltar que a configuração C2 (L1 - Size: 64K, Block Size: 128B; L2 - Size: 2048K, Block Size: 128B.) possuiu um desempenho muito próximo ao apresentado pela C3, inclusive com a diminuição das taxas de miss no cache 2, de acordo com o que foi ressaltado na sessão de analise.


#### Penalidade por hit/miss
Consideramos a penalidade por hit/miss as seguintes:

* Acesso à L1 -> 5 ciclos de latência
* Acesso à L2 -> 28 ciclos de latência
* L1 Miss -> 28 ciclos
* L2 Miss -> 100 ciclos


Com isso, podemos refinar nossa tabela de superescalar/escalar, somando também as bolhas relativas à acesso a memória.

Multiplicamos a quantidade de misses e a quantidade de instruções que lê na memória/cache e somamos a quantidade de penalidade associada aos stalls.

| |**Stalls devido ao acesso à memória**|
|---|---|
|**QSort** | 85787507 |
|**Susan** |391533116|
|**Sha**|210253067|
|**FFT**|676809885|



##Conclusão

Com esses dados em mãos, montamos a seguinte tabela que resume as nossas melhores configurações e experimentos:

||**Escalar**|**Pipeline de 5 estágios**|**BTFNT**|**L1 - Size: 128K, Block Size: 128B; L2 - Size: 2048K, Block Size: 128B** |
|---|---|---|---|---|
| |**QSort**|**Susan**|**Sha**|**FFT**|
| **Ciclos de instruções na pipeline**| 29335439 | 345070391 | 132781865 | 504099557 |
| **Hazard de Dados(ciclos)**| 4580475 | 25423161 | 3325419 | 16038769 |
| **Hazard de Controle(com branch predictor/ciclos)**| 797948 | 453558 | 34439 | 15131438 |
| **Acesso à memória(ciclos)**| 85787507 | 391533116 | 210253067 | 676809885 |
| **Total de Ciclos**| 206288876 | 1154013342 | 556647857 | 1888889534 |
| **Tempo de Execução sem acréscimos\*(s)**| 0.1955695933	| 2.300469273 |	0.8852124333 | 3.360663713 |
| **Tempo de Execução TOTAL(s)**| 1.375259173	| 7.69342228 |	3.710985713 | 5.112672827 |


ps1: para calcular o tempo de execução, consideramos um processador com frequência **150 MHz**, frequencia real de um processador mips com 5 estágios na pipeline(MIPS 4K).

*\*:considerando apenas: (instruções+5=ciclos)/frequência.*



##Referências
[http://www.7-cpu.com/cpu/Octeon2.html]()

[http://www.7-cpu.com/cpu/Mips4K.html]()


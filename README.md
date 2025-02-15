# Simulador de Processador

Este código em C implementa um simulador básico de um processador fictício, onde a memória e as operações aritméticas e lógicas são manipuladas. O programa lê e executa instruções de um arquivo de memória, atualizando o estado do processador com base nas operações encontradas.

## Descrição do Código

### Definições:
- **MEMORIA**: Um vetor que simula a memória do processador, composto por 258 células de 16 bits.
- **Instruções**: O código define 11 operações (NOP, STA, LDA, ADD, OR, AND, NOT, JMP, JN, JZ, HLT) que podem ser executadas pelo processador.
  - `NOP`: No Operation (sem operação).
  - `STA`: Armazena o conteúdo do acumulador (AC) na memória.
  - `LDA`: Carrega o conteúdo da memória no acumulador (AC).
  - `ADD`: Soma o valor da memória com o acumulador (AC).
  - `OR`: Realiza a operação OR bit a bit entre o acumulador (AC) e o valor da memória.
  - `AND`: Realiza a operação AND bit a bit entre o acumulador (AC) e o valor da memória.
  - `NOT`: Inverte os bits do acumulador (AC).
  - `JMP`: Altera o contador de programa (PC) para o endereço especificado na memória.
  - `JN`: Realiza o jump para o endereço na memória se o acumulador (AC) for negativo.
  - `JZ`: Realiza o jump para o endereço na memória se o acumulador (AC) for zero.
  - `HLT`: Para a execução do programa, exibindo os valores finais do acumulador (AC) e do contador de programa (PC).

### Funções:
- **flag()**: Atualiza os flags de Zero (Z) e Negativo (N) com base no valor do acumulador (AC).
- **read()**: Interpreta e executa a instrução encontrada na posição de memória indicada pelo contador de programa (PC). Dependendo da instrução, pode alterar o valor do acumulador (AC), do contador de programa (PC) e da memória.

### Arquivos:
- **arquivo.mem**: Arquivo binário contendo o programa a ser executado pelo simulador. O arquivo é lido e carregado na memória do processador.
- **novo_arquivo.mem**: Arquivo de saída que armazena a memória final após a execução do programa.

### Funcionamento:
1. O arquivo **arquivo.mem** é aberto e lido para preencher a memória do processador.
2. O simulador começa a execução das instruções a partir do endereço indicado pelo contador de programa (PC).
3. O código executa as instruções até encontrar um `HLT`, que termina a execução.
4. O estado final da memória é gravado no arquivo **novo_arquivo.mem**.

## Como Usar:
1. Compile o código usando um compilador C.
2. Prepare um arquivo binário de memória **arquivo.mem** no formato esperado pelo programa.
3. Execute o simulador.
4. O programa vai processar as instruções do arquivo e gerar um novo arquivo **novo_arquivo.mem** com a memória final.

## Exemplo de Execução:
- Crie um arquivo de memória com algumas instruções em formato binário.
- Execute o simulador e observe o conteúdo final da memória no arquivo **novo_arquivo.mem**.

---

## Desenvolvedora: Carolina Gimenez

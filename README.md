# Projetos de Compiladores

Este repositório contém três projetos desenvolvidos como parte de uma aula de compiladores. Cada um dos projetos aborda uma etapa importante do desenvolvimento de um compilador, desde a execução de código até a tradução de código-fonte para código de máquina. A seguir, você encontrará uma descrição detalhada de cada um dos projetos incluídos.

## Projetos:

### 1. **Executor**
Este projeto implementa um simulador básico de processador. Ele lê um arquivo binário de memória, interpreta e executa as instruções armazenadas, e atualiza o estado do processador, incluindo o acumulador (AC) e o contador de programa (PC). Ao final da execução, o estado final da memória é gravado em um novo arquivo.

#### Funcionalidades:
- Leitura de instruções de um arquivo binário de memória.
- Execução de instruções como NOP, STA, LDA, ADD, OR, AND, NOT, JMP, JN, JZ, HLT.
- Atualização de registros (AC, PC) e flags (Zero, Negativo).
- Geração de um arquivo de saída com o estado final da memória.

#### Como Usar:
1. Compile o código.
2. Prepare um arquivo de memória em formato binário.
3. Execute o simulador e analise o arquivo de saída gerado.

> Para mais detalhes, consulte o **README** deste projeto [aqui](link_para_readme).

---

### 2. **Assembler**
O projeto de Assembler é responsável por traduzir um código de assembly (linguagem de baixo nível) para código de máquina (binário) executável pelo simulador de processador. O Assembler interpreta as instruções em linguagem simbólica e as converte em instruções binárias que podem ser executadas pelo **Executor**.

#### Funcionalidades:
- Tradução de código de assembly para código de máquina.
- Suporte para um conjunto básico de instruções (relacionadas ao simulador).
- Geração de um arquivo binário que pode ser carregado no **Executor** para execução.

#### Como Usar:
1. Crie um arquivo de código-fonte em Assembly.
2. Execute o Assembler para gerar o arquivo binário correspondente.
3. Carregue o arquivo binário no **Executor** para execução.

---

### 3. **Compilador**
O projeto de Compilador realiza a tradução de um código-fonte escrito em uma linguagem de programação de alto nível para código de máquina, similar ao que um compilador real faria. Este projeto implementa as etapas de análise léxica, sintática e semântica, além da geração de código intermediário e a tradução final para o formato executável pelo **Executor**.

#### Funcionalidades:
- Análise léxica: Conversão do código-fonte em tokens.
- Análise sintática: Verificação da estrutura do código-fonte.
- Análise semântica: Verificação de tipos e outras regras de consistência.
- Geração de código intermediário: Representação do código-fonte em uma forma mais próxima da máquina.
- Geração de código de máquina: Tradução final para código binário que pode ser executado pelo **Executor**.

#### Como Usar:
1. Crie um arquivo de código-fonte em uma linguagem de alto nível suportada.
2. Compile o código-fonte usando o compilador.
3. O compilador gerará um arquivo binário que pode ser executado no **Executor**.

---

## Licença
Este projeto está licenciado sob a Licença MIT - veja o arquivo [LICENSE](LICENSE) para mais detalhes.

---

## Desenvolvedora: 
Carolina Gimenez

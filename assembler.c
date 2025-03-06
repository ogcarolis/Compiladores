#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "assembler.h"

// Definição de variáveis
uint16_t MEMORIA[258];
FILE *file;

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Erro, número errado de parâmetros!");
    return 1;
  }

  setlocale(LC_ALL, ""); 

  file = fopen(argv[1], "r+");

  if (file == NULL) {
    printf("Não foi possível abrir o arquivo.\n");
    return 1;
  }

  Lexer lexer = {file, 1, NULL, NULL, 0};
  
  MEMORIA[0] = 0x4e03;
  MEMORIA[1] = 0x5244;

  if (executaLexer(&lexer) == 1) {
    return 1;
  }
  
  int tamanhoCode = contarInstrucoes(&lexer);
  printf("Tamanho do .CODE: %d\n\n", tamanhoCode);

  imprimeLexer(&lexer);

  freeLexer(&lexer);

  FILE *arquivo = fopen("bytecode_neander.mem", "wb");

  if (arquivo == NULL) {
    perror("Erro ao criar o arquivo");
    return 1;
  }

  fwrite(MEMORIA, 2, 258, arquivo);
  fclose(arquivo);

  return 0;
}

int executaLexer(Lexer *lexer) {
  // procura o "."
  if (!encontrarPonto(lexer)) {
    freeLexer(lexer);
    return 1;
  }

  // Pegar a palavra data (o . já foi lido)
  if (!verificarBloco(lexer, "DATA")) {
      freeLexer(lexer);
      return 1;
  }

  if (!lerData(lexer)) {
      printf("\033[1;41;30mERRO:\033[0m ao ler o bloco data na linha %d\n", lexer->linha);
      freeLexer(lexer);
      return 1;
  }

  if(!verificarBloco(lexer, "CODE")) {
      freeLexer(lexer);
      return 1;
  }

  if (!encontrarPonto(lexer)) {
      freeLexer(lexer);
      return 1;
  }

  if (!lerOrigem(lexer)) {
      freeLexer(lexer);
      return 1;
  }

  if (!lerInstrucoes(lexer)) {
      freeLexer(lexer);
      return 1;
  }

  if (!salvarData(lexer)) {
      printf("ERRO AQUI");
      freeLexer(lexer);
      return 1;
  }

  if (!salvarCode(lexer)) {
    freeLexer(lexer);
    return 1;
  }

  return 0;
}

int proxLinha(Lexer* lexer) {
  char c;
  while((c = fgetc(lexer->file)) != -1) {
      if (c == '\n') {
          lexer->linha++;
          return 1;
      }
  }

  return 0;
}

int encontrarPonto(Lexer* lexer) {
  char c;
  while((c = fgetc(lexer->file)) != -1) {
      if (c == '.') return 1;
      else if (c == '\n') {
          lexer->linha++;
      }
      else if (c == COMENTARIO && !proxLinha(lexer)) {
          c = -1;
          break;
      }
      else if (c != COMENTARIO && c != SPACE && c != TAB) {
          printf("\033[1;41;30mERRO:\033[0m Caractere invalido \'%c\' na linha %d.\n", c, lexer->linha);
          return 0;
      }
  }

  return 0;
}

int verificarBloco(Lexer* lexer, char* bloco) {
  Palavra palavra = lerPalavra(lexer);
  
  if (strcmp(palavra.texto, bloco) != 0) {
      printf("\033[1;41;30mERRO:\033[0m Estrutura desconhecida %s (não achado o .%s) na linha %d\n", palavra.texto, bloco, lexer->linha);
      return 0;
  }
  
  if (palavra.ultimo == COMENTARIO && proxLinha(lexer)) {
      printf("\033[1;41;30mERRO:\033[0m Fim do arquivo\n");
      return 0;
  }
  else if (palavra.ultimo == '\n') {
      lexer->linha++;
  }
  else if (palavra.ultimo == SPACE || palavra.ultimo == TAB) {
      char c;
      while((c = fgetc(lexer->file)) != -1) {
          if (c == '\n') {
              lexer->linha++;
              break;
          }
          else if (c == COMENTARIO && !proxLinha(lexer)) {
              c = -1;
              break;
          }
          else if (c != COMENTARIO && c != SPACE && c != TAB) {
              printf("Caractere inválido \'%c\' na linha %d.\n", c, lexer->linha);
              return 0;
          }
      }

      if (c == -1) {
          printf("\033[1;41;30mERRO:\033[0m Arquivo incompleto.\n");
          return 0;
      }
  }
  else if (palavra.ultimo != COMENTARIO) {
      printf("\033[1;41;30mERRO:\033[0m caractere inválido (\'%c\') depois da estrutura %s %d\n", palavra.ultimo, bloco, lexer->linha);
      return 0;
  }

  return 1;
}

int lerData(Lexer* lexer) {
  char c;
  int index;
  Data* nova = NULL;

  while((c = fgetc(lexer->file)) != -1) {
      if (c == COMENTARIO && !proxLinha(lexer)) {
          return 0;
      }
      else if (ehLetra(c)) {
          index = 1;
          nova = (Data*) malloc(sizeof(Data));

          if (nova == NULL) {
              printf("Erro ao alocar na memória\n");
              return 0;
          }

          nova->prox = lexer->data;
          nova->token[0] = c;

          // pegar nome de variável
          do {
              c = fgetc(lexer->file);
              if (c == COMENTARIO || c == '\n') {
                  free(nova);
                  return 1;
              }
              else if (c != SPACE && c != TAB) {
                  nova->token[index] = c;
                  index++;
              }
          } while(c != -1 && c != TAB && c != SPACE && index < TOKEN_SIZE);

          if (c == -1) {
              free(nova);
              return 0;
          }

          nova->token[index] = '\0';

          // pegar tipo
          Palavra palavra = lerPalavra(lexer);

          if (palavra.ultimo != SPACE && palavra.ultimo != TAB) {
              free(nova);
              return 0;
          }

          strncpy(nova->tipo, palavra.texto, palavra.tamanho);

          // pegar valor
          palavra = lerPalavra(lexer);

          if (palavra.ultimo != SPACE && palavra.ultimo != TAB && palavra.ultimo != '\n' && palavra.ultimo != COMENTARIO) {
              free(nova);
              return 0;
          }

          if (palavra.tamanho > 2) {
              free(nova);
              return 0;
          }

          // valor pode ser um hexa ou indefinido (?)  
          if (
              (!ehHexadecimal(palavra.texto[0]) && palavra.texto[0] != '?') || 
              (palavra.tamanho == 2 && !ehHexadecimal(palavra.texto[1]))
          ) {
              free(nova);
              return 0;
          }
          
          if (palavra.texto[0] == '?') {
              nova->vazio = 1;
          }
          else {
              nova->valor = (uint8_t)strtol(palavra.texto, NULL, 16);
              nova->vazio = 0;
          }
          

          if (palavra.ultimo == COMENTARIO && !proxLinha(lexer)) {
              return 1;
          }
          else if (palavra.ultimo == '\n') {
              lexer->linha++;
          }
          else if (c != COMENTARIO) {
              do {
                  c = fgetc(lexer->file);
                  if (c != SPACE && c != TAB && c != COMENTARIO && c != '\n') {
                      free(nova);
                      return 0;
                  }
              } while(c != -1 && c != COMENTARIO && c != '\n');
  
              if (c == -1) {
                  free(nova);
                  return 0;
              }

              if (c == COMENTARIO && !proxLinha(lexer)) {
                  free(nova);
                  return 0;
              }
              else if (c == '\n') {
                  lexer->linha++;
              }
          }
          
          lexer->data = nova;
      }
      else if (c == '\n') lexer->linha++;
      else if (c == '.') return 1;
      else if (c != TAB && c != SPACE) return 0;
  }

  return 0;
}

Palavra lerPalavra(Lexer* lexer) {
  Palavra palavra;
  palavra.tamanho = 0;
  palavra.ultimo = '\0';

  char c;

  while((c = fgetc(lexer->file)) != -1) {
      if (c == '\n' || c == TAB || c == SPACE || c == COMENTARIO || palavra.tamanho == TOKEN_SIZE) {
          palavra.ultimo = c;
          palavra.texto[palavra.tamanho] = '\0';
          return palavra;
      }
      
      palavra.texto[palavra.tamanho] = c;
      palavra.tamanho++;
  }

  palavra.texto[0] = '\0';

  return palavra;
}

Mnemonico salvarMnemonico(char* token) {
  if (strcmp(token, "NOP") == 0) return NOP;
  else if (strcmp(token, "STA") == 0) return STA;
  else if (strcmp(token, "LDA") == 0) return LDA;
  else if (strcmp(token, "ADD") == 0) return ADD;
  else if (strcmp(token, "AND") == 0) return AND;
  else if (strcmp(token, "OR") == 0) return OR;
  else if (strcmp(token, "NOT") == 0) return NOT;
  else if (strcmp(token, "JMP") == 0) return JMP;
  else if (strcmp(token, "JN") == 0) return JN;
  else if (strcmp(token, "JZ") == 0) return JZ;
  else if (strcmp(token, "HLT") == 0) return HLT;
  else return DESCONHECIDO;
}

int NaoPossuiValor(Mnemonico m) {
  return m == NOP || m == NOT || m == HLT || m == DESCONHECIDO;
}

int lerOrigem(Lexer* lexer) {
  Palavra palavra = lerPalavra(lexer);

  if (strcmp(palavra.texto, "ORG") != 0) {
      printf("\033[1;41;30mERRO:\033[0m estrutura errada depois de .CODE %s (não encontrado o .ORG) na linha %d\n", palavra.texto, lexer->linha);
      return 0;
  }
  
  if (palavra.ultimo != SPACE && palavra.ultimo != TAB) {
      printf("\033[1;41;30mERRO:\033[0m valor de origem nao achado\n");
      return 0;
  }

  palavra = lerPalavra(lexer);

  if (palavra.tamanho > 2 || !ehHexadecimal(palavra.texto[0]) || (palavra.tamanho == 2 && !ehHexadecimal(palavra.texto[1]))) {
      printf("\033[1;41;30mERRO:\033[0m Posição invalida\n");
      return 0;
  }

  lexer->origem = (uint8_t)strtol(palavra.texto, NULL, 16);

  if (palavra.ultimo == '\n') lexer->linha++;
  else if (palavra.ultimo == COMENTARIO && !proxLinha(lexer)) {
      printf("\033[1;41;30mERRO:\033[0m Fim do arquivo\n");
      return 0;
  }
  else if (palavra.ultimo != COMENTARIO && palavra.ultimo != SPACE && palavra.ultimo != TAB) {
      printf("\033[1;41;30mERRO:\033[0m Caractere invalido \'%c\' na linha %d\n", palavra.ultimo, lexer->linha);
      return 0;
  }

  return 1;
}

int lerInstrucoes(Lexer* lexer) {
  char c;
  int index;
  Instrucao* nova = NULL;

  while((c = fgetc(lexer->file)) != -1) {
      if (c == COMENTARIO && !proxLinha(lexer)) {
          printf("\033[1;41;30mERRO:\033[0m Fim do arquivo\n");
          return 0;
      }
      else if (ehLetra(c)) {
          nova = (Instrucao*) malloc(sizeof(Instrucao));

          if (nova == NULL) {
              printf("Erro ao alocar na memória\n");
              return 0;
          }

          nova->prox = NULL;
          Instrucao* aux = lexer->code;

          while(aux != NULL && aux->prox != NULL) {
              aux = aux->prox;
          }

          char mnemonico[MAX_MNEMONICO_SIZE + 1];
          mnemonico[0] = c;

          index = 1;
          // pegar mnemonico
          do {
              c = fgetc(lexer->file);
              if (ehLetra(c)) {
                  mnemonico[index] = c;
                  index++;
              }
              else if (c != -1 && c != SPACE && c != TAB) {
                  free(nova);
                  printf("\033[1;41;30mERRO:\033[0m Mnemonico invalido na linha %d\n", lexer->linha);
                  return 0;
              }
          } while(c != -1 && c != TAB && c != SPACE && index < MAX_MNEMONICO_SIZE + 1);

          mnemonico[index] = '\0';

          nova->mnemonico = salvarMnemonico(mnemonico);

          if (nova->mnemonico == DESCONHECIDO) {
              printf("\033[1;41;30mERRO:\033[0m Mnemonico desconhecido na linha %d!\n", lexer->linha);
              free(nova);
              return 0;
          }

          if (!NaoPossuiValor(nova->mnemonico)) {
              // pegar valor
              Palavra palavra = lerPalavra(lexer);

              if (palavra.tamanho == 0) {
                  printf("\033[1;41;30mERRO:\033[0m o mnemonico %s precisa de um complemento\n", mnemonico);
                  free(nova);
                  return 0;
              }

              strncpy(nova->token, palavra.texto, palavra.tamanho);

              if (palavra.ultimo == COMENTARIO && !proxLinha(lexer)) {
                  printf("\033[1;41;30mERRO:\033[0m Fim do arquivo\n");
                  return 1;
              }
              else if (palavra.ultimo == '\n') {
                  lexer->linha++;
              }
              else if (palavra.ultimo == SPACE || palavra.ultimo == TAB) {
                  do {
                      c = fgetc(lexer->file);
                      if (c != SPACE && c != TAB && c != COMENTARIO && c != '\n') {
                          printf("\033[1;41;30mERRO:\033[0m %c muitos argumentos na linha %d\n", c, lexer->linha);
                          free(nova);
                          return 0;
                      }
                  } while(c != -1 && c != COMENTARIO && c != '\n');
      
                  if (c == -1) {
                      printf("\033[1;41;30mERRO:\033[0m Fim do arquivo\n");
                      free(nova);
                      return 0;
                  }

                  if (c == COMENTARIO && !proxLinha(lexer)) {
                      printf("\033[1;41;30mERRO:\033[0m Fim do arquivo\n");
                      free(nova);
                      return 0;
                  }
                  else if (c == '\n') {
                      lexer->linha++;
                  }
              }
              else if (palavra.ultimo != COMENTARIO) {
                  printf("\033[1;41;30mERRO:\033[0m Erro na linha %d\n", lexer->linha);
                  free(nova);
                  return 0;
              }
          }
          else {
              nova->token[0] = '\0';
          }
          
          if (aux == NULL) {
            lexer->code = nova;
          }
          else {
            aux->prox = nova;
          }
      }
      else if (c == '\n') lexer->linha++;
      else if (c != TAB && c != SPACE) return 0;
  }

  return 1;
}

void imprimeLexer(Lexer* lexer) {
  printf("Linha: %d\n", lexer->linha);

  printf("Data:\n");
  Data* auxData = lexer->data;
  while (auxData != NULL) {
      if (auxData->vazio) {
          printf("- %s %s %s\n", auxData->token, auxData->tipo, "Sem valor");
      }
      else {
          printf("- %s %s %2x\n", auxData->token, auxData->tipo, auxData->valor);
      }
      auxData = auxData->prox;
  }

  printf("Code (%x):\n", lexer->origem);
  Instrucao* auxInstrucao = lexer->code;
  while (auxInstrucao != NULL) {
      printf("- %x %s\n", auxInstrucao->mnemonico, auxInstrucao->token);
      auxInstrucao = auxInstrucao->prox;
  }
}

void freeLexer(Lexer* lexer) {
  if (lexer == NULL) return;

  while (lexer->data != NULL) {
      Data* aux = lexer->data->prox;
      free(lexer->data);
      lexer->data = aux;
  }

  while (lexer->code != NULL) {
      Instrucao* aux = lexer->code->prox;
      free(lexer->code);
      lexer->code = aux;
  }

  if (lexer->file)
      fclose(lexer->file);
}

int contarInstrucoes(Lexer* lexer) {
  Instrucao* instrucao = lexer->code;
  int count = 0;

  while (instrucao != NULL) {
      count++;
      if (instrucao->mnemonico == HLT) {
          break;
      }
      else if (!NaoPossuiValor(instrucao->mnemonico)) {
        count++;
      }

      instrucao = instrucao->prox;
  }

  return count;
}

int salvarData(Lexer* lexer) {
  Data* auxData = lexer->data;

  int tamanhoCode = contarInstrucoes(lexer);
  uint16_t posicao = (uint16_t)(tamanhoCode + OFFSET + lexer->origem);

  while (auxData != NULL) {
      if (!auxData->vazio && tamanhoCode + posicao + 1 < 258) {
          MEMORIA[posicao] = (uint8_t)auxData->valor;
          auxData->posMemoria = posicao;
          printf("Posicao Var %s: %x\n", auxData->token, auxData->posMemoria);
          posicao++;
      }
      else if (tamanhoCode + posicao + 1 >= 258) {
          printf("\033[1;41;30mERRO:\033[0m Tentativa de escrita fora do limite do arquivo!\n");
          return 0;
      }
      auxData = auxData->prox;
  }

  return 1;
}

int salvarCode(Lexer* lexer) {
  Instrucao* auxInstrucao = lexer->code;

  int tamanhoCode = contarInstrucoes(lexer);
  uint16_t dataPos = (uint16_t)(tamanhoCode + OFFSET - 1);
  uint16_t posicao = lexer->origem;

  while (auxInstrucao != NULL) {
      if (posicao + OFFSET < 258) {
          MEMORIA[posicao + OFFSET] = (uint16_t)auxInstrucao->mnemonico;
          posicao++;

          if(!NaoPossuiValor(auxInstrucao->mnemonico)) {
            size_t tamanho = strlen(auxInstrucao->token);

            Data* auxData = lexer->data;
            uint16_t posicaoVar = dataPos;
            int encontrou = 0;

            while (auxData != NULL) {
              if (strcmp(auxData->token, auxInstrucao->token) == 0) {
                MEMORIA[posicao + OFFSET] = (uint16_t)auxData->posMemoria - OFFSET;
                printf("INST %x VAR %s POS %x\n", auxInstrucao->mnemonico, auxData->token, auxData->posMemoria);
                posicao++;
                encontrou = 1;
                break;
              }
              posicaoVar++;
              auxData = auxData->prox;
            }

            if (tamanho <= 2 && ehHexadecimal(auxInstrucao->token[0]) && (tamanho != 2 || ehHexadecimal(auxInstrucao->token[1])) && !encontrou) {
              MEMORIA[posicao + OFFSET] = (uint8_t)strtol(auxInstrucao->token, NULL, 16);
              posicao++;
            }
          }
      }
      else if (posicao + OFFSET >= 258) {
          printf("\033[1;41;30mERRO:\033[0m Tentativa de escrita fora do limite do arquivo!\n");
          return 0;
      }
      auxInstrucao = auxInstrucao->prox;

  }

  return 1;
}
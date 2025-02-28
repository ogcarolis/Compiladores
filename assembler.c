#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definição de variáveis
uint16_t MEMORIA[256];
FILE *file;

// Definições gerais
#define DOT '.'
#define SPACE ' '
#define TAB '\t'
#define NEWLINE '\n'
#define NUMBERS {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'}
#define SEMICOLON ';'

// Definição de mnemonicos
char *mnemonicos_duplos[] = {"STA", "LDA", "ADD", "OR", "AND", "JMP", "JN", "JZ"};
uint16_t codigos_duplos[] = {16, 32, 48, 64, 80, 128, 144, 160};
char *mnemonicos_simples[] = {"NOP", "NOT", "HLT"};
uint16_t codigos_simples[] = {0, 96, 240};


// Struct dos Tokens
typedef struct Tokens {
    int tipo;
    int valor;
} Tokens;

void instrucao(char *mnemonico, uint16_t valor, uint16_t *pos_mnemonico, uint16_t *pos_valor); 
int assembler(FILE *file);

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Erro, número errado de parâmetros!");
    return 1;
  }

  file = fopen(argv[1], "rb");

  if (file == NULL) {
    printf("Não foi possível abrir o arquivo.\n");
    return 1;
  }

  assembler(file);

  return 0;
}

void instrucao(char *mnemonico, uint16_t valor, uint16_t *pos_mnemonico, uint16_t *pos_valor) {
    for (int i = 0; i < 11; i++) {
        if(strcmp(mnemonico, mnemonicos_duplos[i]) == 0) {
            *pos_mnemonico = codigos_simples[i];
            *pos_valor = valor;
            printf("Instrução %s com valor %hu, escrita com sucesso!", mnemonico, valor);
            return;
        }
        else if (strcmp(mnemonico, mnemonicos_simples[i]) == 0) {
            *pos_mnemonico = codigos_simples[i];
            printf("Instrução %s com valor %hu, escrita com sucesso!", mnemonico);
            return;
        }
    }
}

// int assembler(FILE *file){
//     int pos = 0;
//     char linha[100];

//     while(fgets(linha, sizeof(linha), file) != NULL) {

//     }
// }
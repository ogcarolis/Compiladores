#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definições gerais
#define DOT '.'
#define SPACE ' '
#define TAB '\t'
#define NEWLINE '\n'
#define NUMBERS {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'}
#define SEMICOLON ';'

// Enum de comandos
typedef enum {
    NOP = 0,
} Comandos;

// Struct das instruções
typedef struct Instrucoes {
    Comandos comando;
    char *token;
    Instrucoes *prox;
} Instrucoes;

void lexer(char caracter); 

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Erro, número errado de parâmetros!");
    return 1;
  }

  // file = fopen(argv[1], "rb");

  return 0;
}

void lexer(char caracter) {
    switch (caracter) {
            case SPACE:
                break;
            case NEWLINE:
                break;
            case TAB:
                break;
            case SEMICOLON:
                break;
        }
}
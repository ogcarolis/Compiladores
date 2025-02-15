#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint16_t MEMORIA[258];

#define NOP 0
#define STA 16
#define LDA 32
#define ADD 48
#define OR 64
#define AND 80
#define NOT 96
#define JMP 128
#define JN 144
#define JZ 160
#define HLT 240

#define OFFSET 2

int AC = 0;
int PC = 0;
bool Z;
bool N;
FILE *file;

void flag();
int read(uint16_t line);

int main(void) {
  file = fopen("arquivo.mem", "rb");

  if (file == NULL) {
    perror("Erro ao abrir o arquivo");
    fclose(file);
    return 1;
  }


  size_t items = fread(MEMORIA, 2, 258, file);
  if (items != 258) {
    printf("Erro ao ler o arquivo!\n");
    fclose(file);
    return 1;
  }
  fclose(file);

  int start = 1;

  while (start == 1) {
    start = read(MEMORIA[PC + OFFSET]);
  }

  FILE *arquivo = fopen("novo_arquivo.mem", "w");

  if (arquivo == NULL) {
    perror("Erro ao criar o arquivo");
    return 1;
  }

  fwrite(MEMORIA, 2, 258, arquivo);
  fclose(arquivo);
  
  return 0;
}

void flag() {
  Z = AC == 0;
  N = AC < 0;
}

int read(uint16_t line) {
  switch (line) {
  case NOP:
    PC++;
    break;
  case STA:
    MEMORIA[MEMORIA[PC + 3] + OFFSET] = AC;
    PC += 2;
    break;
  case LDA:
    AC = MEMORIA[MEMORIA[PC + 3] + OFFSET];
    PC += 2;
    break;
  case ADD:
    AC = AC + MEMORIA[MEMORIA[PC + 3] + OFFSET];
    PC += 2;
    break;
  case OR:
    AC = AC | MEMORIA[MEMORIA[PC + 3] + OFFSET];
    PC += 2;
    break;
  case AND:
    AC = AC & MEMORIA[MEMORIA[PC + 3] + OFFSET];
    PC += 2;
    break;
  case NOT:
    AC = ~AC;
    PC++;
    break;
  case JMP:
    PC = MEMORIA[MEMORIA[PC + 3] + OFFSET];
    break;
  case JN:
    flag();
    if (N) {
      PC = MEMORIA[MEMORIA[PC + 3] + OFFSET];
    } else {
      PC += 2;
    }
    break;
  case JZ:
    flag();
    if (Z) {
      PC = MEMORIA[MEMORIA[PC + 3] + OFFSET];
    } else {
      PC += 2;
    }
    break;
  case HLT:
    printf("AC: %d\n", AC);
    printf("PC: %d\n", PC);
    return 0;
    break;
  default:
    printf("Instrução não reconhecida");
    return 0;
    break;
  }

  return 1;
}
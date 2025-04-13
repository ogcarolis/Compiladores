#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct variavel {
    char *nome; 
    char *valor; 
    int tem_valor;
    struct variavel *prox;
} Variavel;

Variavel *variaveis = NULL;

int expressaoValida(char *valor) {
    if (valor == NULL) return 0;

    int temOperador = 0;
    
    for (int i = 0; valor[i] != '\0'; i++) {
        char c = valor[i];
        
        if (!((c >= 'a' && c <= 'z') ||
              (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') ||
              (c == '+') || (c == '-') ||
              (c == ' '))) {
            return 0; // achou caractere inválido
        }

        if (c == '+' || c == '-') {
            temOperador = 1; // encontrou pelo menos um operador
        }
    }

    return temOperador; // só é válido se tiver operador
}


int calcularExpressaoSimples(const char *expr) {
    char *copia = strdup(expr);  // Faz uma cópia da expressão
    char *token = strtok(copia, "+-");
    int resultado = 0;
    int primeiro = 1;
    const char *p = expr;  // ponteiro para o original

    while (token) {
        int valor = atoi(token);

        if (primeiro) {
            resultado = valor;
            primeiro = 0;
        } else {
            // Descobre se o operador anterior era + ou -
            while (*p && (*p == ' ' || isdigit(*p))) p++;  // pula dígitos e espaços
            if (*p == '+') {
                resultado += valor;
            } else if (*p == '-') {
                resultado -= valor;
            }
            p++;  // avança depois do operador
        }

        token = strtok(NULL, "+-");
    }

    free(copia);
    return resultado;
}

void addVar(const char *nome, const char *valor) {
    // Verifica se já existe
    Variavel *atual = variaveis;
    while (atual) {
        if (strcmp(atual->nome, nome) == 0) return;  // Caso a variável já exista ele retorna um erro
        atual = atual->prox;
    }

    printf("%s: %s\n", nome, valor);

    Variavel *nova = malloc(sizeof(Variavel));
    nova->nome = strdup(nome); 
    nova->tem_valor = (valor != NULL);
    nova->valor = valor ? strdup(valor) : NULL;
    nova->prox = variaveis;
    variaveis = nova;
}

void gerarDataCode(FILE *saida) {
    fprintf(saida, ".DATA\n");
    Variavel *v = variaveis;
    while (v) {
        printf("Variavel %s -> Valor %s\n", v->nome, v->valor);
        if (v->tem_valor && strspn(v->valor, "0123456789") == strlen(v->valor)) {
            printf("entrou aqui no data\n");
            int valorInt = atoi(v->valor);
            fprintf(saida, "%s DB %02X\n", v->nome, valorInt);
            printf("%s DB %02X\n", v->nome, valorInt);
        } else if (v->tem_valor && strspn(v->valor, "0123456789+-") == strlen(v->valor)) {
            int valorNovo = calcularExpressaoSimples(v->valor);
            fprintf(saida, "%s DB %02X\n", v->nome, valorNovo);
            printf("%s DB %02X\n", v->nome, valorNovo);
        }
        else {
            fprintf(saida, "%s DB ?\n", v->nome);
            printf("%s DB ?\n", v->nome);
        }
        v = v->prox;
    }
}

void gerarInstrucaoExpr(FILE *saida, const char *expr, const char *destino) {
    printf("Expressao: %s\n", expr);
    char *copia = strdup(expr);  // Faz uma cópia da expressão
    char *token = strtok(copia, "+-");
    const char *p = expr; 
    int primeiro = 1;

    while (token) {
        // Remover espaços antes da variável (trim)
        while (*token == ' ') token++;
        char *valor = token;

        if (primeiro) {
            primeiro = 0;
            fprintf(saida, "LDA %s\n", valor);
            printf("LDA %s\n", valor);
        } else {
            // Descobre se o operador anterior era + ou -
            while (*p && *p != '+' && *p != '-') p++;  // pula caracteres e espaços
            if (*p == '+') {
                fprintf(saida, "ADD %s\n", valor);
                printf("ADD %s\n", valor);
            } else if (*p == '-') {
                printf("NEGATIVO AJUSTAR! %s\n", valor);
            }
            p++;  // avança depois do operador
        }
        token = strtok(NULL, "+-");
    }

    fprintf(saida, "STA %s\n", destino);
    free(copia);
}

void gerarCode(FILE *saida) {
    fprintf(saida, ".CODE\n.ORG 0\n");
    Variavel *v = variaveis;

    while (v) {
        printf("Valor da expressao: %s\n", v->valor);
        if (v->tem_valor && expressaoValida(v->valor)) {
            printf("Entrou aqui no code uhu\n");
            gerarInstrucaoExpr(saida, v->valor, v->nome);
        } else if (v->tem_valor && strspn(v->valor, "0123456789+-") == strlen(v->valor)) { 
            v = v->prox;
            continue; 
        }
        else if (v->tem_valor) {
            printf("Valor diferente do esperado! %s\n", v->valor);
        }
        v = v->prox;
    }

    fprintf(saida, "STA RES\n");
    fprintf(saida, "HLT\n");
}

void leituraCodigo(FILE *entrada) {
    char linha[256];
    int inicio = 0;

    rewind(entrada);
    while (fgets(linha, sizeof(linha), entrada)) {
        linha[strcspn(linha, "\r\n")] = 0;
        if (strncmp(linha, "PROGRAMA", 8) == 0) {
            continue; // Pula a linha do cabeçalho
        }
        if (strstr(linha, "INICIO")) {
            inicio = 1;
            continue;
        }
        if (!inicio) continue; 

        if(strstr(linha, "FIM")) { 
            printf("Arquivo finalizado!\n");
            break; 
        }

        if (linha[0] == '\0') continue;

        char var[32], expr[128];
        if (sscanf(linha, "%s = %[^\n]", var, expr) == 2 && inicio) {
            printf("Entrou! Linha: %s\n", linha);
            printf("%s = %s\n", var, expr);
            addVar(var, expr);
        }
    }

    addVar("RES", NULL); 
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <arquivo.gpa>\n", argv[0]);
        return 1;
    }

    FILE *entrada = fopen(argv[1], "r");
    if (!entrada) {
        perror("Erro ao abrir arquivo .gpa");
        return 1;
    }

    // Cria o arquivo final
    FILE *saida = fopen("output.asm", "w");
    if (!saida) {
        perror("Erro ao criar output.asm");
        return 1;
    }
    
    leituraCodigo(entrada);

    // Escreve a seção .DATA
    gerarDataCode(saida);
    gerarCode(saida);

    // Copia o conteúdo de .CODE do buffer temporário
    
    fclose(entrada);
    fclose(saida);

    printf("Compilado com sucesso para output.asm\n");
    return 0;
}


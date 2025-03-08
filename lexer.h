#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

#define MAX_LINHA 256 // Define o tamanho máximo de uma linha no analisador léxico

// O enum TipoTok representa os diferentes tipos de tokens que podem ser reconhecidos pelo analisador léxico.
typedef enum {
    TOK_NULO, // Token nulo
    TOK_INST, // Token de instrução
    TOK_NUM,  // Token numérico
    TOK_DIR,  // Token de diretiva
    TOK_EOL,  // Token de fim de linha
    TOK_FIM   // Token de fim do arquivo
} TipoTok;

// A estrutura Token representa um token identificado pelo analisador léxico,
// contendo seu tipo, valor e a linha em que foi encontrado.
typedef struct {
    TipoTok tipo;
    char* val;
    int linha;
} Token;

// A estrutura Lexer representa o analisador léxico, contendo um arquivo de entrada,
// um buffer para armazenar a linha atual, um ponteiro para a posição de leitura atual,
// o número da linha em análise e um indicador de comentário.
typedef struct {
    FILE* arq;
    char linha[MAX_LINHA];
    char* ptr;
    int num_linha;
    int coment;
} Lexer;

// A função novo_lex recebe um ponteiro para um arquivo e retorna um novo analisador léxico inicializado.
Lexer* novo_lex(FILE* arq);

// A função del_lex libera a memória alocada para um analisador léxico.
void del_lex(Lexer* lex);

// A função prox_tok retorna o próximo token identificado pelo analisador léxico.
Token* prox_tok(Lexer* lex);

// A função del_tok libera a memória alocada para um token.
void del_tok(Token* tok);

// A função delim recebe um caractere e retorna 1 se for um delimitador e 0 caso contrário.
int delim(char c);

// A função coment recebe uma string e retorna 1 se for um comentário e 0 caso contrário.
int coment(const char* lin);

// A função hex recebe uma string e retorna 1 se representar um número hexadecimal válido e 0 caso contrário.
int hex(const char* str);

// A função hex2int recebe uma string representando um número hexadecimal e retorna seu valor inteiro correspondente.
int hex2int(const char* hex);

// A função num recebe uma string representando um número e retorna seu valor inteiro.
int num(const char* str);

#endif 

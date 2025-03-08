#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

// A função novo_lex recebe um ponteiro para um arquivo e retorna
// um novo analisador léxico inicializado.
Lexer* novo_lex(FILE* arq) {
    Lexer* lex = (Lexer*)malloc(sizeof(Lexer));
    if (!lex) return NULL;
    
    lex->arq = arq;
    lex->linha[0] = '\0';
    lex->ptr = lex->linha;
    lex->num_linha = 0;
    lex->coment = 0;
    
    return lex;
}

// A função del_lex libera a memória alocada para um analisador léxico.
void del_lex(Lexer* lex) {
    if (lex) {
        free(lex);
    }
}

// A função delim recebe um caractere c e retorna 1 se for um delimitador
// (espaço, tabulação, quebra de linha ou ponto e vírgula) e 0 caso contrário.
int delim(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == ';');
}

// A função coment recebe uma string lin e retorna 1 se a linha for um comentário
// (começar com um ponto e vírgula, ignorando espaços iniciais) e 0 caso contrário.
int coment(const char* lin) {
    while (*lin && (*lin == ' ' || *lin == '\t')) {
        lin++;
    }
    return (*lin == ';');
}

// A função hex recebe uma string str e retorna 1 se a string for um número hexadecimal válido
// e 0 caso contrário.
int hex(const char* str) {
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        str += 2;
    }
    while (*str) {
        if (!((*str >= '0' && *str <= '9') || 
              (*str >= 'a' && *str <= 'f') || 
              (*str >= 'A' && *str <= 'F'))) {
            return 0;
        }
        str++;
    }
    return 1;
}

// A função hex2int recebe uma string representando um número hexadecimal e 
// retorna seu valor inteiro correspondente.
int hex2int(const char* hex) {
    int res = 0;
    while (*hex) {
        res *= 16;
        if (*hex >= '0' && *hex <= '9') {
            res += *hex - '0';
        } else if (*hex >= 'A' && *hex <= 'F') {
            res += *hex - 'A' + 10;
        } else if (*hex >= 'a' && *hex <= 'f') {
            res += *hex - 'a' + 10;
        }
        hex++;
    }
    return res;
}

// A função num recebe uma string representando um número e retorna seu valor inteiro.
// A string pode representar um número decimal ou hexadecimal.
int num(const char* str) {
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        return strtol(str + 2, NULL, 16);
    }
    if (hex(str)) {
        return hex2int(str);
    }
    return atoi(str);
}

// A função prox_tok recebe um analisador léxico e retorna o próximo token do arquivo.
Token* prox_tok(Lexer* lex) {
    Token* tok = (Token*)malloc(sizeof(Token));
    if (!tok) return NULL;
    
    tok->tipo = TOK_NULO;
    tok->val = NULL;
    tok->linha = lex->num_linha;
    
    if (*(lex->ptr) == '\0') {
        if (fgets(lex->linha, MAX_LINHA, lex->arq) == NULL) {
            tok->tipo = TOK_FIM;
            return tok;
        }
        lex->ptr = lex->linha;
        lex->num_linha++;
        
        if (coment(lex->linha)) {
            free(tok);
            return prox_tok(lex);
        }
        
        tok->linha = lex->num_linha;
    }
    
    while (*(lex->ptr) && delim(*(lex->ptr))) {
        if (*(lex->ptr) == ';') {
            *(lex->ptr) = '\0';
            tok->tipo = TOK_EOL;
            return tok;
        }
        (lex->ptr)++;
    }
    
    if (*(lex->ptr) == '\0') {
        tok->tipo = TOK_EOL;
        return tok;
    }
    
    char* ini = lex->ptr;
    while (*(lex->ptr) && !delim(*(lex->ptr))) {
        (lex->ptr)++;
    }
    
    char save = *(lex->ptr);
    if (*(lex->ptr)) {
        *(lex->ptr) = '\0';
        (lex->ptr)++;
    }
    
    if (ini[0] == '.') {
        tok->tipo = TOK_DIR;
    } else if (isdigit(ini[0]) || 
              (ini[0] == '0' && (ini[1] == 'x' || ini[1] == 'X'))) {
        tok->tipo = TOK_NUM;
    } else {
        tok->tipo = TOK_INST;
    }
    
    tok->val = strdup(ini);
    
    if (save != '\0') {
        *(lex->ptr - 1) = save;
    }
    
    return tok;
}

// A função del_tok libera a memória alocada para um token.
void del_tok(Token* tok) {
    if (tok) {
        if (tok->val) {
            free(tok->val);
        }
        free(tok);
    }
}

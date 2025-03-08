#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include "lexer.h"

#define TAM_MEM 512 // Define o tamanho máximo da memória

// A estrutura Instrucao representa uma instrução da linguagem, contendo
// seu nome e o código de operação correspondente.
typedef struct {
    char* nome;
    unsigned short op;
} Instrucao;

// A estrutura Parser representa o analisador sintático, contendo
// o analisador léxico associado, a memória do programa, endereços de controle,
// uma indicação da seção atual (.DATA ou .CODE) e o conjunto de instruções suportadas.
typedef struct {
    Lexer* lex;
    unsigned short mem[TAM_MEM];
    int addr_atual;
    int addr_max;
    int sec_dados;
    const Instrucao* instr;
    int qtd_instr;
} Parser;

// A função novo_parser recebe um analisador léxico, um conjunto de instruções e sua quantidade,
// e retorna um novo analisador sintático inicializado.
Parser* novo_parser(Lexer* lex, const Instrucao* instr, int qtd_instr);

// A função liberar_parser libera a memória alocada para um analisador sintático.
void liberar_parser(Parser* p);

// A função executar_parser processa o código-fonte e preenche a memória do analisador sintático.
// Retorna 1 em caso de sucesso e 0 em caso de erro.
int executar_parser(Parser* p);

// A função localizar_instrucao recebe um vetor de instruções, sua quantidade e um nome de instrução,
// e retorna o índice da instrução no vetor ou -1 se não for encontrada.
int localizar_instrucao(const Instrucao* instr, int qtd, const char* nome);

// A função gravar_saida escreve a memória do analisador sintático em um arquivo binário e gera um dump.
// Retorna 1 em caso de sucesso e 0 em caso de erro.
int gravar_saida(Parser* p, const char* nome_saida);

#endif

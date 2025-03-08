#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"

// O vetor instr contém as instruções da linguagem e seus respectivos códigos de operação.
Instrucao instr[] = {
    {"NOP", 0x0000},
    {"STA", 0x0010},
    {"LDA", 0x0020},
    {"ADD", 0x0030},
    {"OR",  0x0040},
    {"AND", 0x0050},
    {"NOT", 0x0060},
    {"JMP", 0x0080},
    {"JN",  0x0090},
    {"JZ",  0x00A0},
    {"HLT", 0x00F0}
};

// A constante QTD_INSTR armazena a quantidade de instruções definidas no vetor instr.
const int QTD_INSTR = sizeof(instr) / sizeof(Instrucao);

// A função main recebe os argumentos da linha de comando e executa o analisador léxico e sintático.
// Retorna 0 em caso de sucesso e 1 em caso de erro.
int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s arquivo_entrada arquivo_saida\n", argv[0]);
        return 1;
    }
    
    FILE* arq_entrada = fopen(argv[1], "r");
    if (!arq_entrada) {
        fprintf(stderr, "Erro ao abrir entrada: %s\n", argv[1]);
        return 1;
    }
    
    Lexer* lex = novo_lex(arq_entrada);
    Parser* p = novo_parser(lex, instr, QTD_INSTR);
    
    if (!executar_parser(p)) {
        fprintf(stderr, "Erro ao processar arquivo.\n");
        liberar_parser(p);
        del_lex(lex);
        fclose(arq_entrada);
        return 1;
    }
    
    if (!gravar_saida(p, argv[2])) {
        fprintf(stderr, "Erro ao escrever saída.\n");
        liberar_parser(p);
        del_lex(lex);
        fclose(arq_entrada);
        return 1;
    }
    
    liberar_parser(p);
    del_lex(lex);
    fclose(arq_entrada);
    
    return 0;
}

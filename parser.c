#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "parser.h"

// A função novo_parser recebe um analisador léxico, um conjunto de instruções e sua quantidade,
// e retorna um novo analisador sintático inicializado.
Parser* novo_parser(Lexer* lex, const Instrucao* instr, int qtd_instr) {
    Parser* p = (Parser*)malloc(sizeof(Parser));
    if (!p) return NULL;
    
    p->lex = lex;
    memset(p->mem, 0, TAM_MEM * sizeof(unsigned short));
    p->addr_atual = 0;
    p->addr_max = 0;
    p->sec_dados = 0;
    p->instr = instr;
    p->qtd_instr = qtd_instr;
    
    return p;
}

// A função liberar_parser libera a memória alocada para um analisador sintático.
void liberar_parser(Parser* p) {
    if (p) {
        free(p);
    }
}

// A função localizar_instrucao recebe um vetor de instruções, sua quantidade e um nome de instrução,
// e retorna o índice da instrução no vetor ou -1 se não for encontrada.
int localizar_instrucao(const Instrucao* instr, int qtd, const char* nome) {
    for (int i = 0; i < qtd; i++) {
        if (strcmp(instr[i].nome, nome) == 0) {
            return i;
        }
    }
    return -1;
}

// A função executar_parser recebe um analisador sintático e executa a análise do código-fonte.
// Retorna 1 em caso de sucesso e 0 em caso de erro.
int executar_parser(Parser* p) {
    Token* tok;
    
    while ((tok = prox_tok(p->lex))->tipo != TOK_FIM) {
        if (tok->tipo == TOK_EOL) {
            del_tok(tok);
            continue;
        }
        
        if (tok->tipo == TOK_DIR) {
            if (strcmp(tok->val, ".DATA") == 0) {
                p->sec_dados = 1;
            } else if (strcmp(tok->val, ".CODE") == 0) {
                p->sec_dados = 0;
                p->addr_atual = 0;
            }
            del_tok(tok);
            continue;
        }
        
        if (p->sec_dados) {
            if (tok->tipo != TOK_NUM) {
                fprintf(stderr, "Erro: O número esperado na seção .DATA, linha %d\n", tok->linha);
                del_tok(tok);
                return 0;
            }
            
            int endereco = num(tok->val);
            del_tok(tok);
            
            tok = prox_tok(p->lex);
            if (tok->tipo != TOK_NUM) {
                fprintf(stderr, "Erro: O valor esperado após endereço, linha %d\n", tok->linha);
                del_tok(tok);
                return 0;
            }
            
            int valor = num(tok->val);
            del_tok(tok);
            p->mem[endereco] = valor;
            
            if (endereco > p->addr_max) {
                p->addr_max = endereco;
            }
        } else {
            if (tok->tipo != TOK_INST) {
                fprintf(stderr, "Erro: Ocorreu um erro mnemonico esperado na seção .CODE, linha %d\n", tok->linha);
                del_tok(tok);
                return 0;
            }
            
            int idx = localizar_instrucao(p->instr, p->qtd_instr, tok->val);
            if (idx == -1) {
                fprintf(stderr, "Erro: Ocorreu um erro mnemonico desconhecido '%s', linha %d\n", tok->val, tok->linha);
                del_tok(tok);
                return 0;
            }
            
            p->mem[p->addr_atual] = p->instr[idx].op;
            
            if (strcmp(tok->val, "NOP") != 0 && strcmp(tok->val, "HLT") != 0) {
                del_tok(tok);
                tok = prox_tok(p->lex);
                if (tok->tipo != TOK_NUM) {
                    fprintf(stderr, "Erro: Operando esperado, linha %d\n", tok->linha);
                    del_tok(tok);
                    return 0;
                }
                
                int operando = num(tok->val);
                p->addr_atual++;
                p->mem[p->addr_atual] = operando;
            }
            
            del_tok(tok);
            p->addr_atual++;
            
            if (p->addr_atual > p->addr_max) {
                p->addr_max = p->addr_atual;
            }
        }
    }
    
    del_tok(tok);
    return 1;
}

// A função gravar_saida recebe um analisador sintático e um nome de arquivo,
// e escreve o código de máquina gerado em um arquivo binário e um dump em texto.
// Retorna 1 em caso de sucesso e 0 em caso de erro.
int gravar_saida(Parser* p, const char* nome_saida) {
    FILE* arquivo_saida;

    unsigned short mem_temp[TAM_MEM] = {0};
    for (int i = 0; i <= p->addr_max; i++) {
        mem_temp[i] = p->mem[i];
    }

    p->mem[0] = 0x4e03;
    p->mem[1] = 0x5244;

    for (int i = 0; i <= p->addr_max; i++) {
        p->mem[i + 2] = mem_temp[i];
    }

    p->addr_max += 2;

    arquivo_saida = fopen(nome_saida, "wb");
    if (!arquivo_saida) {
        fprintf(stderr, "Erro ao abrir arquivo de saída: %s\n", nome_saida);
        return 0;
    }

    fwrite(p->mem, 2, p->addr_max + 1, arquivo_saida);
    fclose(arquivo_saida);

    char nome_dump[256];
    snprintf(nome_dump, sizeof(nome_dump), "%s.dump", nome_saida);

    FILE* dump_saida = fopen(nome_dump, "w");
    if (!dump_saida) {
        fprintf(stderr, "Erro ao criar arquivo dump: %s\n", nome_dump);
        return 0;
    }

    int tam_alinhado = (p->addr_max + 3) & ~3;
    if (tam_alinhado < 128) {
        tam_alinhado = 128;
    }

    for (int i = 0; i <= tam_alinhado; i += 4) {
        fprintf(dump_saida, "%07o", i * 2);
        for (int j = 0; j < 4 && i + j <= tam_alinhado; j++) {
            fprintf(dump_saida, " %04x", p->mem[i + j]);
        }
        fprintf(dump_saida, "\n");

        int encontrou_dados = 0;
        int k;
        for (k = i + 4; k <= tam_alinhado; k += 4) {
            for (int j = 0; j < 4 && k + j <= tam_alinhado; j++) {
                if (p->mem[k + j] != 0) {
                    encontrou_dados = 1;
                    break;
                }
            }
            if (encontrou_dados) break;
        }

        if (!encontrou_dados && i + 4 <= tam_alinhado) {
            fprintf(dump_saida, "*\n");
            i = k - 4;
        }
    }

    fprintf(dump_saida, "%07o\n", (tam_alinhado + 1) * 2);
    fclose(dump_saida);

    printf("A montagem foi finalizada com sucesso.\n");
    printf("Arquivo binário: %s\n", nome_saida);
    printf("Arquivo dump: %s\n", nome_dump);

    return 1;
}

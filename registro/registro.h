#ifndef REGISTRO_H
#define REGISTRO_H
#include <stdio.h>

typedef struct {
    char removido;
    int proximo;
    int codEstacao;
    int codLinha;
    int codProxEstacao;
    int distProxEstacao;
    int codLinhaIntegra;
    int codEstIntegra;
    int tamNomeEstacao;
    char nomeEstacao[100];
    int tamNomeLinha;
    char nomeLinha[100];
} Registro;

void parse_linha_csv(char *linha, Registro *r);
void escreve_registro_bin(FILE *bin, Registro *r);

#endif
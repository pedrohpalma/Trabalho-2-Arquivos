#ifndef CABECALHO_H
#define CABECALHO_H

#include <stdio.h>

typedef struct {
    char status;
    int topo;
    int proxRRN;
    int nroEstacoes;
    int nroParesEstacao;
} Cabecalho;

void ler_cabecalho(FILE *fp, Cabecalho *c);
void escrever_cabecalho(FILE *fp, Cabecalho *c);
void atualizar_status(FILE *fp, char status);


#endif
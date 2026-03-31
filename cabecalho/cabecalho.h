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



void initCabecalho(Cabecalho *c);
void lerCabecalho(FILE *fp, Cabecalho *c);
void escreverCabecalho(FILE *fp, Cabecalho *c);
void atualizarStatus(FILE *fp, char novoStatus);


#endif
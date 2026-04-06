#ifndef CABECALHO_H
#define CABECALHO_H
#include <stdio.h>

#define TAM_CABECALHO 17

typedef struct {
    char status;           // '0' inconsistente, '1' consistente
    int topo;              // Byte offset de registro logicamente removido (-1 inicial)
    int proxRRN;           // Próximo RRN disponível
    int nroEstacoes;       // Qtd de estações únicas
    int nroParesEstacao;   // Qtd de pares únicos (codEstacao, codProxEstacao)
} Cabecalho;

void initCabecalho(Cabecalho *c);
void escreveCabecalho(FILE *bin, Cabecalho *c);
void atualizaCabecalho(FILE *bin, Cabecalho *c);
int leCabecalho(FILE *bin, Cabecalho *c);
void atualizaContagemEstacoes(FILE *bin, Cabecalho *c);

#endif
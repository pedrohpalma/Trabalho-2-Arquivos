#ifndef CABECALHO_H
#define CABECALHO_H
#include <stdio.h>

//operaçoes utilizadas sob o cabecalho

#define TAM_CABECALHO 17

typedef struct {
    char status;           // 0 inconsistente, 1 consistente
    int topo;              // byte offset de registro logicamente removido (-1 inicial)
    int proxRRN;           // Prox RRN disponível
    int nroEstacoes;       // Qtd de estações unicas
    int nroParesEstacao;   // Qtd de pares unicos (codEstacao, codProxEstacao)
} Cabecalho;

void initCabecalho(Cabecalho *c);
void escreveCabecalho(FILE *bin, Cabecalho *c);
void atualizaCabecalho(FILE *bin, Cabecalho *c);
int leCabecalho(FILE *bin, Cabecalho *c);
void atualizaContagemEstacoes(FILE *bin, Cabecalho *c);

#endif
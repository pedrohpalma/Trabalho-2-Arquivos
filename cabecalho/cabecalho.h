#ifndef CABECALHO_H
#define CABECALHO_H
#include <stdio.h>

typedef struct {
    char status;           // '0' inconsistente, '1' consistente
    int topo;              // Byte offset de registro logicamente removido (-1 inicial)
    int proxRRN;           // Próximo RRN disponível
    int nroEstacoes;       // Qtd de estações únicas
    int nroParesEstacao;   // Qtd de pares únicos (codEstacao, codProxEstacao)
} Cabecalho;

void inicializa_cabecalho(Cabecalho *c);
void escreve_cabecalho(FILE *bin, Cabecalho *c);
void atualiza_cabecalho(FILE *bin, Cabecalho *c);
int le_cabecalho(FILE *bin, Cabecalho *c);

#endif
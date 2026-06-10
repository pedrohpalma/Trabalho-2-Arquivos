#ifndef CABECALHO_ARVORE_B_H
#define CABECALHO_ARVORE_B_H

#include <stdio.h>

#define TAM_CABECALHO_ARVORE_B 17

/*
 * Metadados do arquivo de indice arvore-B.
 * status controla consistencia: '0' = em escrita/inconsistente,
 * '1' = consistente. noRaiz, topo e proxRRN armazenam RRNs.
 */
typedef struct
{
    char status;
    int noRaiz;
    int topo;
    int proxRRN;
    int nroNos;
} CabecalhoArvoreB;

CabecalhoArvoreB criarCabecalhoArvoreBVazio();
int lerCabecalhoArvoreB(FILE *arquivoIndice, CabecalhoArvoreB *cabecalho);
int escreverCabecalhoArvoreB(FILE *arquivoIndice, CabecalhoArvoreB *cabecalho);
int atualizarStatusArvoreB(FILE *arquivoIndice, char status);

#endif

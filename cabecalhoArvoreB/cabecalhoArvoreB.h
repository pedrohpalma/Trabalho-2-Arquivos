#ifndef CABECALHO_ARVORE_B_H
#define CABECALHO_ARVORE_B_H

typedef struct cabecalhoArvoreB
{
    char status; // 0 inconsistente, 1 consistente
    int noRaiz;  // RRN da raiz
    int topo;    // RRN do topo da pilha de removidos
    int proxRRN; // RRN do prox espaço livre disponível
    int nroNos;  // numero de nos da arvore
} cabecalhoArvoreB;

#endif

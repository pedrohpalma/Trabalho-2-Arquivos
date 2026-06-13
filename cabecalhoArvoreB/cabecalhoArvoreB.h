#ifndef CABECALHO_ARVORE_B_H
#define CABECALHO_ARVORE_B_H

#include <stdio.h>

#define TAM_CABECALHO_ARVORE_B 17

/*
 * status: 0 = Inconsistente / 1 = Consistente
 * noRaiz: RRN que aponta para o nó raiz da árvoreB (armazena -1 se a árvore estiver vazia)
 * topo: RRN do topo da pilha de nós removidos (-1 se não houver removidos)
 * proxRRN: próximo RRN disponível para a criação de um novo nó no fim do arquivo
 * nroNos: quantidade total de nós ativos atualmente armazenados na árvoreB
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
#ifndef CABECALHO_ARVORE_B_H
#define CABECALHO_ARVORE_B_H

#include <stdio.h>

#define TAM_CABECALHO_ARVORE_B 17

// Struct do cabecalho do arquivo de indice da Arvore-B
typedef struct
{
    char status;
    int noRaiz;
    int topo;
    int proxRRN;
    int nroNos;
} CabecalhoArvoreB;

// Inicializa a estrutura do cabecalho na memoria com valores padrao (arvore vazia).
CabecalhoArvoreB criarCabecalhoArvoreBVazio();

// Le os 17 bytes do cabecalho do arquivo para a memoria.
int lerCabecalhoArvoreB(FILE *arquivoIndice, CabecalhoArvoreB *cabecalho);

// Grava a estrutura do cabecalho no byte 0 do arquivo de indice.
int escreverCabecalhoArvoreB(FILE *arquivoIndice, CabecalhoArvoreB *cabecalho);

// Altera diretamente o byte de status no arquivo.
int atualizarStatusArvoreB(FILE *arquivoIndice, char status);

#endif
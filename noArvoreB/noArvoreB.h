#ifndef NO_ARVORE_B_H
#define NO_ARVORE_B_H

#include <stdio.h>

#define TAM_NO_ARVORE_B 53
#define ORDEM_ARVORE_B 4
#define MAX_CHAVES_ARVORE_B 3

typedef struct
{
    char removido;
    int proximo;
    int tipoNo;
    int nroChaves;
    int C[3];
    int PR[3];
    int P[4];
} NoArvoreB;

NoArvoreB criarNoArvoreBVazio(int tipoNo);
long calcularOffsetNoArvoreB(int rrn);
int lerNoArvoreB(FILE *arquivoIndice, int rrn, NoArvoreB *no);
int escreverNoArvoreB(FILE *arquivoIndice, int rrn, NoArvoreB *no);
int buscarPosicaoNo(NoArvoreB *no, int chave);
void inserirChaveOrdenadaNo(NoArvoreB *no, int chave, int referencia, int filhoDireita);
void inicializarCamposNaoUsadosComMenosUm(NoArvoreB *no);

#endif
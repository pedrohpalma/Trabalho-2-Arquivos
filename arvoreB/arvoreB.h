#ifndef ARVORE_B_H
#define ARVORE_B_H

#include <stdio.h>

int criarArquivoIndiceArvoreB(FILE *arquivoIndice);                 // cria arvore b no arquivo de indices
int buscarArvoreB(FILE *arquivoIndice, int chave, int *referencia); // busca em arvore B
int inserirArvoreB(FILE *arquivoIndice, int chave, int referencia); // insercao em arvore B
int removerArvoreB(FILE *arquivoIndice, int chave);                 // remocao em arvore B

#endif

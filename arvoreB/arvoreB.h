#ifndef ARVORE_B_H
#define ARVORE_B_H

#include <stdio.h>

int criarArquivoIndiceArvoreB(FILE *arquivoIndice);
int buscarArvoreB(FILE *arquivoIndice, int chave, int *referencia);
int inserirArvoreB(FILE *arquivoIndice, int chave, int referencia);
int removerArvoreB(FILE *arquivoIndice, int chave);

#endif

#ifndef ARQUIVO_H
#define ARQUIVO_H
#include <stdio.h>

FILE* abrirArquivo(char *nome, char *modo);
void fecharArquivo(FILE *arq);

#endif
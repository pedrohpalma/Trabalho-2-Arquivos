#ifndef ARQUIVO_H
#define ARQUIVO_H
#include <stdio.h>

FILE* abrir_arquivo(char *nome, char *modo);
void fechar_arquivo(FILE *arq);

#endif
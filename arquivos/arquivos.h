#ifndef ARQUIVO_H
#define ARQUIVO_H
#include <stdio.h>


 // Abre o arquivo no modo indicado (ex: rb, wb) e retorna o ponteiro FILE*
FILE* abrirArquivo(char *nome, char *modo);

// Fecha o arquivo apontado por arq e libera os recursos associados a ele
void fecharArquivo(FILE *arq);

#endif

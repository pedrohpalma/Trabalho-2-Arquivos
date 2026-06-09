#ifndef ARQUIVO_H
#define ARQUIVO_H
#include <stdio.h>

/*
 * Abre o arquivo de nome 'nome' no modo indicado por 'modo' (ex: "rb", "wb").
 * Retorna o ponteiro FILE* em caso de sucesso, ou NULL em caso de falha.
 * Encapsula fopen para centralizar a abertura de arquivos no projeto.
 */
FILE* abrirArquivo(char *nome, char *modo);

/*
 * Fecha o arquivo apontado por 'arq', ignorando ponteiros NULL com segurança.
 * Encapsula fclose para uso consistente em todo o projeto.
 */
void fecharArquivo(FILE *arq);

#endif

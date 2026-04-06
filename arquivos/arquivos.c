#include "../arquivos/arquivos.h"
#include <stdio.h>

//fazem a mesma coisa que o fopen e fclose. foram implementados porque achávamos no início do projeto 
//que teríamos que fazer alguns outros ajustes na forma de abertura do arquivo que exigiriam funções separadas.
//no fim, deixamos desse jeito pois já havíamos implementado.

FILE* abrirArquivo(char *nome, char *modo) {
    return fopen(nome, modo);
}

void fecharArquivo(FILE *arq) {
    if (arq != NULL) {
        fclose(arq);
    }
}
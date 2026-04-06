#include "../arquivos/arquivos.h"
#include <stdio.h>

FILE* abrirArquivo(char *nome, char *modo) {
    return fopen(nome, modo);
}

void fecharArquivo(FILE *arq) {
    if (arq != NULL) {
        fclose(arq);
    }
}
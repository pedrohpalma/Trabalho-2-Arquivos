#include "../arquivos/arquivos.h"
#include <stdio.h>

FILE* abrir_arquivo(char *nome, char *modo) {
    return fopen(nome, modo);
}

void fechar_arquivo(FILE *arq) {
    if (arq != NULL) {
        fclose(arq);
    }
}
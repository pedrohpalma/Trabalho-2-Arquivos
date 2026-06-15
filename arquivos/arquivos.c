#include "../arquivos/arquivos.h"
#include <stdio.h>


//Funções encapsulam fopen e fclose para centralizar o acesso a arquivos no projeto

// Abre o arquivo 'nome' no modo indicado e retorna o ponteiro; NULL em caso de falha
FILE* abrirArquivo(char *nome, char *modo) {
    return fopen(nome, modo);
}

// Fecha o arquivo somente se o ponteiro nao for NULL, evitando comportamento indefinido
void fecharArquivo(FILE *arq) {
    if (arq != NULL) {
        fclose(arq);
    }
}

#include "../arquivos/arquivos.h"
#include <stdio.h>

/*
 * Estas funcoes encapsulam fopen e fclose para centralizar o acesso
 * a arquivos no projeto. Foram criadas prevendo possiveis ajustes futuros
 * na logica de abertura (ex.: tratamento de erros customizado, logging),
 * mesmo que atualmente se comportem de forma identica as chamadas padrao.
 */

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

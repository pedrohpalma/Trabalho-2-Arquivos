#include <stdio.h>
#include "read.h"
#include "../arquivos/arquivos.h"
#include "../cabecalho/cabecalho.h"
#include "../registro/registro.h"


// Funcao Select *: percorre o binario e imprime todos os registros nao removidos
void func2(char *arqBin) {
    FILE *bin = abrirArquivo(arqBin, "rb");
    if (!bin) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    Cabecalho c;

    // Verifica consistencia do arquivo antes de prosseguir
    if (!leCabecalho(bin, &c)) {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        return;
    }

    // proxRRN == 0 significa que nenhum registro foi inserido ainda
    if (c.proxRRN == 0) {
        printf("Registro inexistente.\n");
        fecharArquivo(bin);
        return;
    }

    int encontrados = 0;
    Registro r;
    int status_leitura;

    // Itera sequencialmente; leRegistroBin retorna 2 para removidos, 0 para EOF
    while ((status_leitura = leRegistroBin(bin, &r)) != 0) {
        if (status_leitura == 2) continue; // pula registros logicamente removidos

        encontrados++;
        imprimeRegistro(&r);
    }

    // Se todos os registros existentes estiverem removidos logicamente
    if (encontrados == 0) {
        printf("Registro inexistente.\n");
    }

    fecharArquivo(bin);
}

#include <stdio.h>
#include "func7.h"
#include "../arquivos/arquivos.h"
#include "../cabecalho/cabecalho.h"
#include "../registro/registro.h"
#include "../utilitarios/utils.h"
#include "../arvoreB/arvoreB.h"
#include "../cabecalhoArvoreB/cabecalhoArvoreB.h"

void func7(char *arqEntrada, char *arqIndice) {
    FILE *bin = abrirArquivo(arqEntrada, "rb");
    if (!bin) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    Cabecalho c;
    if (!leCabecalho(bin, &c)) {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        return;
    }

    FILE *indice = abrirArquivo(arqIndice, "wb+");
    if (!indice) {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        return;
    }

    if (!criarArquivoIndiceArvoreB(indice)) {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        fecharArquivo(indice);
        return;
    }

    fseek(bin, TAM_CABECALHO, SEEK_SET);

    Registro r;
    int status_leitura;

    while (1) {
        long offsetRegistro = ftell(bin);
        status_leitura = leRegistroBin(bin, &r);

        if (status_leitura == 0) {
            break;
        }

        if (status_leitura == 2) {
            continue;
        }

        if (!inserirArvoreB(indice, r.codEstacao, (int)offsetRegistro)) {
            printf("Falha no processamento do arquivo.\n");
            fecharArquivo(bin);
            fecharArquivo(indice);
            return;
        }
    }

    if (!atualizarStatusArvoreB(indice, '1')) {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        fecharArquivo(indice);
        return;
    }

    fecharArquivo(bin);
    fecharArquivo(indice);

    BinarioNaTela(arqIndice);
}

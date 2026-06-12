#include <stdio.h>
#include "func9.h"
#include "../arquivos/arquivos.h"
#include "../cabecalho/cabecalho.h"
#include "../registro/registro.h"
#include "../utilitarios/utils.h"
#include "../arvoreB/arvoreB.h"
#include "../cabecalhoArvoreB/cabecalhoArvoreB.h"

static long inserirRegistroDadosFunc9(FILE *bin, Cabecalho *c, Registro *r) {
    long offset;

    if (c->topo != -1) {
        int rrn_reuso = c->topo;
        int proximo_removido;

        offset = TAM_CABECALHO + (rrn_reuso * TAM_REGISTRO);
        fseek(bin, offset + 1, SEEK_SET);

        if (fread(&proximo_removido, 4, 1, bin) != 1) {
            return -1;
        }

        c->topo = proximo_removido;
        fseek(bin, offset, SEEK_SET);
        escreveRegistroBin(bin, r);
    } else {
        offset = TAM_CABECALHO + (c->proxRRN * TAM_REGISTRO);
        fseek(bin, offset, SEEK_SET);
        escreveRegistroBin(bin, r);
        c->proxRRN++;
    }

    fseek(bin, 0, SEEK_SET);
    escreveCabecalho(bin, c);
    fflush(bin);

    return offset;
}

void func9(char *arqEntrada, char *arqIndice, int n) {
    FILE *bin = abrirArquivo(arqEntrada, "rb+");
    if (!bin) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    FILE *indice = abrirArquivo(arqIndice, "rb+");
    if (!indice) {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        return;
    }

    Cabecalho c;
    if (!leCabecalho(bin, &c)) {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        fecharArquivo(indice);
        return;
    }

    CabecalhoArvoreB cabecalhoIndice;
    if (!lerCabecalhoArvoreB(indice, &cabecalhoIndice) || cabecalhoIndice.status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        fecharArquivo(indice);
        return;
    }

    Registro registros[n];
    int deveInserir[n];
    for (int i = 0; i < n; i++) {
        int referenciaExistente;

        leRegistroTeclado(&registros[i]);
        deveInserir[i] = 1;

        if (buscarArvoreB(indice, registros[i].codEstacao, &referenciaExistente)) {
            deveInserir[i] = 0;
        }

        for (int j = 0; j < i; j++) {
            if (deveInserir[j] && registros[j].codEstacao == registros[i].codEstacao) {
                deveInserir[i] = 0;
            }
        }
    }

    c.status = '0';
    fseek(bin, 0, SEEK_SET);
    escreveCabecalho(bin, &c);

    cabecalhoIndice.status = '0';
    if (!escreverCabecalhoArvoreB(indice, &cabecalhoIndice)) {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        fecharArquivo(indice);
        return;
    }

    for (int i = 0; i < n; i++) {
        long offsetInserido;

        if (!deveInserir[i]) {
            continue;
        }

        offsetInserido = inserirRegistroDadosFunc9(bin, &c, &registros[i]);
        if (offsetInserido == -1) {
            printf("Falha no processamento do arquivo.\n");
            fecharArquivo(bin);
            fecharArquivo(indice);
            return;
        }

        if (!inserirArvoreB(indice, registros[i].codEstacao, (int)offsetInserido)) {
            printf("Falha no processamento do arquivo.\n");
            fecharArquivo(bin);
            fecharArquivo(indice);
            return;
        }
    }

    atualizaContagemEstacoes(bin, &c);
    atualizaCabecalho(bin, &c);

    if (!atualizarStatusArvoreB(indice, '1')) {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        fecharArquivo(indice);
        return;
    }

    fecharArquivo(bin);
    fecharArquivo(indice);

    BinarioNaTela(arqEntrada);
    BinarioNaTela(arqIndice);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func9.h"
#include "../arquivos/arquivos.h"
#include "../cabecalho/cabecalho.h"
#include "../registro/registro.h"
#include "../utilitarios/utils.h"
#include "../arvoreB/arvoreB.h"
#include "../cabecalhoArvoreB/cabecalhoArvoreB.h"

// insere registro no arquivo de dados priorizando reaproveitamento de removidos ou colocando no final
static long inserirRegistroDadosFunc9(FILE *bin, Cabecalho *c, Registro *r) {
    long offset;

    if (c->topo != -1) {
        // recupera o rrn do topo da pilha de removidos
        int rrn_reuso = c->topo;
        int proximo_removido;

        offset = TAM_CABECALHO + (rrn_reuso * TAM_REGISTRO);
        
        // pula byte de removido para pegar o proximo da pilha
        fseek(bin, offset + 1, SEEK_SET);

        if (fread(&proximo_removido, 4, 1, bin) != 1) {
            return -1;
        }

        c->topo = proximo_removido;
        fseek(bin, offset, SEEK_SET);
        escreveRegistroBin(bin, r);
    } else {
        // grava direto no final do arquivo e atualiza rrn
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

// le registros do teclado e insere no binario e na arvoreB garantindo chaves unicas
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
    // checa se a arvoreB esta consistente antes de comecar
    if (!lerCabecalhoArvoreB(indice, &cabecalhoIndice) || cabecalhoIndice.status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        fecharArquivo(indice);
        return;
    }

    // cria vetores para lidar com a leitura em lote e validacao de duplicatas
    Registro registros[n];
    int deveInserir[n];
    
    for (int i = 0; i < n; i++) {
        int referenciaExistente;

        leRegistroTeclado(&registros[i]);
        deveInserir[i] = 1;

        // checa se registro ja existe no arquivo de indice
        if (buscarArvoreB(indice, registros[i].codEstacao, &referenciaExistente)) {
            deveInserir[i] = 0;
        }

        // checa se insercao atual é duplicada em relacao as leituras anteriores do mesmo lote
        for (int j = 0; j < i; j++) {
            if (deveInserir[j] && registros[j].codEstacao == registros[i].codEstacao) {
                deveInserir[i] = 0;
            }
        }
    }

    // marca os status como inconsistentes enquanto escreve em lote
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

        // atualiza o indice arvoreB com o novo registro inserido
        if (!inserirArvoreB(indice, registros[i].codEstacao, (int)offsetInserido)) {
            printf("Falha no processamento do arquivo.\n");
            fecharArquivo(bin);
            fecharArquivo(indice);
            return;
        }
    }

    // recria os cabecalhos com os status corretos e dados atualizados
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
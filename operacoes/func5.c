#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func5.h"
#include "../arquivos/arquivos.h"
#include "../cabecalho/cabecalho.h"
#include "../registro/registro.h"
#include "../utilitarios/utils.h"


// Funcao Insert Into: insere 'n' registros, reaproveitando espacos de removidos quando disponivel
void func5(char *arqBin, int n) {
    FILE *bin = abrirArquivo(arqBin, "rb+");
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

    // Marca inconsistente durante o processo de insercao
    c.status = '0';
    fseek(bin, 0, SEEK_SET);
    fwrite(&c.status, 1, 1, bin);
    fflush(bin);

    for (int i = 0; i < n; i++) {
        Registro r;
        leRegistroTeclado(&r); // le os campos do novo registro da entrada padrao

        if (c.topo != -1) {
            // Reuso de espaco: ha registros removidos na pilha; escreve no RRN do topo
            int rrn_reuso = c.topo;
            long offset = TAM_CABECALHO + (rrn_reuso * TAM_REGISTRO);

            // Le o ponteiro 'proximo' do registro removido antes de sobrescreve-lo
            int proximo_removido;
            fseek(bin, offset + 1, SEEK_SET);
            fread(&proximo_removido, 4, 1, bin);

            c.topo = proximo_removido; // desempilha: topo passa a apontar para o proximo removido
            fseek(bin, offset, SEEK_SET);
            escreveRegistroBin(bin, &r);
        } else {
            // Sem removidos disponiveis: insere ao final do arquivo
            long offset = TAM_CABECALHO + (c.proxRRN * TAM_REGISTRO);
            fseek(bin, offset, SEEK_SET);
            escreveRegistroBin(bin, &r);
            c.proxRRN++;
        }
    }

    // Reconta estacoes e pares unicos apos as insercoes
    atualizaContagemEstacoes(bin, &c);

    atualizaCabecalho(bin, &c);
    fecharArquivo(bin);

    BinarioNaTela(arqBin);
}

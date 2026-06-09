#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "write.h"
#include "../arquivos/arquivos.h"
#include "../cabecalho/cabecalho.h"
#include "../registro/registro.h"
#include "../utilitarios/utils.h"


// Funcao Create Table: le o .csv, converte para binario e escreve o cabecalho
void func1(char *arqEntrada, char *arqSaida) {
    FILE *csv = abrirArquivo(arqEntrada, "r");
    if (!csv) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    FILE *bin = abrirArquivo(arqSaida, "wb");
    if (!bin) {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(csv);
        return;
    }

    Cabecalho c;
    initCabecalho(&c);
    escreveCabecalho(bin, &c); // inicia cabecalho zerado (status='0')

    char linha[1024];
    fgets(linha, sizeof(linha), csv); // ignora a linha de cabecalho do CSV

    // Arrays auxiliares para verificar unicidade durante a leitura
    char estacoes_unicas[2000][100];
    int pares_unicos[2000][2];

    while (fgets(linha, sizeof(linha), csv)) {
        Registro r;
        lerRegistroCSV(linha, &r);
        escreveRegistroBin(bin, &r);

        // Verifica se o nomeEstacao ja foi contabilizado antes de incrementar
        int estacao_existe = 0;
        for (int i = 0; i < c.nroEstacoes; i++) {
            if (strcmp(estacoes_unicas[i], r.nomeEstacao) == 0) {
                estacao_existe = 1;
                break;
            }
        }
        if (!estacao_existe) {
            strcpy(estacoes_unicas[c.nroEstacoes], r.nomeEstacao);
            c.nroEstacoes++;
        }

        // Verifica unicidade do par (codEstacao, codProxEstacao) — ignora sem proxima estacao
        if (r.codProxEstacao != -1) {
            int par_existe = 0;
            for (int i = 0; i < c.nroParesEstacao; i++) {
                if (pares_unicos[i][0] == r.codEstacao && pares_unicos[i][1] == r.codProxEstacao) {
                    par_existe = 1;
                    break;
                }
            }
            if (!par_existe) {
                pares_unicos[c.nroParesEstacao][0] = r.codEstacao;
                pares_unicos[c.nroParesEstacao][1] = r.codProxEstacao;
                c.nroParesEstacao++;
            }
        }

        c.proxRRN++;
    }

    // Atualiza cabecalho com status='1' (consistente) e valores finais contabilizados
    atualizaCabecalho(bin, &c);

    fecharArquivo(csv);
    fecharArquivo(bin);

    BinarioNaTela(arqSaida);
}


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

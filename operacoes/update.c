#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "update.h"
#include "../arquivos/arquivos.h"
#include "../cabecalho/cabecalho.h"
#include "../registro/registro.h"
#include "../utilitarios/utils.h"


// Funcao Update: atualiza campos de registros que satisfacam os criterios de busca
void func6(char *arqBin, int n) {
    FILE *bin = abrirArquivo(arqBin, "rb+");
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

    // Marca inconsistente enquanto realiza atualizacoes
    c.status = '0';
    fseek(bin, 0, SEEK_SET);
    fwrite(&c.status, 1, 1, bin);
    fflush(bin);

    for (int i = 0; i < n; i++) {
        // Leitura dos criterios de busca
        int m;
        scanf("%d", &m);
        char camposBusca[m][50], valoresStrBusca[m][100];
        int valoresIntBusca[m], isNuloBusca[m];

        for (int j = 0; j < m; j++) {
            scanf("%s", camposBusca[j]);
            if (strcmp(camposBusca[j], "nomeEstacao") == 0 || strcmp(camposBusca[j], "nomeLinha") == 0) {
                ScanQuoteString(valoresStrBusca[j]);
                isNuloBusca[j] = (strcmp(valoresStrBusca[j], "") == 0);
                valoresIntBusca[j] = -1;
            } else {
                char temp[50];
                scanf("%s", temp);
                if (strcmp(temp, "NULO") == 0) {
                    isNuloBusca[j] = 1;
                    valoresIntBusca[j] = -1;
                } else {
                    isNuloBusca[j] = 0;
                    valoresIntBusca[j] = atoi(temp);
                }
            }
        }

        // Leitura dos campos a serem atualizados e seus novos valores
        int p;
        scanf("%d", &p);
        char camposAtualiza[p][50], valoresStrAtualiza[p][100];
        int valoresIntAtualiza[p], isNuloAtualiza[p];

        for (int j = 0; j < p; j++) {
            scanf("%s", camposAtualiza[j]);
            if (strcmp(camposAtualiza[j], "nomeEstacao") == 0 || strcmp(camposAtualiza[j], "nomeLinha") == 0) {
                ScanQuoteString(valoresStrAtualiza[j]);
                isNuloAtualiza[j] = (strcmp(valoresStrAtualiza[j], "") == 0);
                valoresIntAtualiza[j] = -1;
            } else {
                char temp[50];
                scanf("%s", temp);
                if (strcmp(temp, "NULO") == 0) {
                    isNuloAtualiza[j] = 1;
                    valoresIntAtualiza[j] = -1;
                } else {
                    isNuloAtualiza[j] = 0;
                    valoresIntAtualiza[j] = atoi(temp);
                }
            }
        }

        // rewind limpa a flag de EOF antes do fseek para garantir leitura correta
        rewind(bin);
        fseek(bin, TAM_CABECALHO, SEEK_SET);

        Registro r;
        int status_leitura;
        int rrn_atual = 0;

        while ((status_leitura = leRegistroBin(bin, &r)) != 0) {
            if (status_leitura == 2) {
                rrn_atual++; // avanca o RRN mesmo para registros removidos
                continue;
            }

            int match = 1;
            for (int j = 0; j < m; j++) {
                if (!atendeCriterio(&r, camposBusca[j], valoresStrBusca[j], valoresIntBusca[j], isNuloBusca[j])) {
                    match = 0;
                    break;
                }
            }

            if (match) {
                // Aplica todas as atualizacoes de campos ao registro lido
                for (int j = 0; j < p; j++) {
                    autualizaCampo(&r, camposAtualiza[j], valoresStrAtualiza[j], valoresIntAtualiza[j], isNuloAtualiza[j]);
                }

                long offset_atual = TAM_CABECALHO + (rrn_atual * TAM_REGISTRO);

                // Reescreve o registro completo no mesmo offset
                fseek(bin, offset_atual, SEEK_SET);
                escreveRegistroBin(bin, &r);
                fflush(bin);

                // Reposiciona para o inicio do proximo registro apos reescrita
                fseek(bin, offset_atual + TAM_REGISTRO, SEEK_SET);
            }
            rrn_atual++;
        }
    }

    // Finaliza: restaura consistencia e descarrega buffer
    atualizaCabecalho(bin, &c);
    fflush(bin);
    fecharArquivo(bin);

    BinarioNaTela(arqBin);
}

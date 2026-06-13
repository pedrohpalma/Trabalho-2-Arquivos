#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func6.h"
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
        // 1. Leitura dos criterios de busca (Where) padronizada
        int m;
        scanf("%d", &m);
        CampoBusca criteriosBusca[m];
        int possuiCodBusca, valorCodBusca;
        lerCriteriosBusca(criteriosBusca, m, &possuiCodBusca, &valorCodBusca);

        // 2. Leitura dos campos a serem atualizados e seus novos valores (Set)
        // Reaproveita a mesma struct e funcao utilitaria de leitura
        int p;
        scanf("%d", &p);
        CampoBusca criteriosAtualiza[p];
        int ignoraCod, ignoraVal; 
        lerCriteriosBusca(criteriosAtualiza, p, &ignoraCod, &ignoraVal);

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

            // Verifica criterios da clausula WHERE
            if (registroSatisfazCriterios(&r, criteriosBusca, m)) {
                
                // Aplica todas as atualizacoes de campos ao registro lido rodando o array de Set
                for (int j = 0; j < p; j++) {
                    atualizaCampo(&r, criteriosAtualiza[j].nomeCampo, 
                                      criteriosAtualiza[j].valorStr, 
                                      criteriosAtualiza[j].valorInt, 
                                      criteriosAtualiza[j].isNulo);
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

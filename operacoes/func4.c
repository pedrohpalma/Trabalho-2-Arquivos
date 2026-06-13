#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func4.h"
#include "../arquivos/arquivos.h"
#include "../cabecalho/cabecalho.h"
#include "../registro/registro.h"
#include "../utilitarios/utils.h"


// Funcao Delete From: realiza 'n' remocoes logicas com multiplos criterios
void func4(char *arqBin, int n) {
    FILE *bin = abrirArquivo(arqBin, "r+b");
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

    // Marca o arquivo como inconsistente durante a operacao de remocao
    c.status = '0';
    fseek(bin, 0, SEEK_SET);
    fwrite(&c.status, 1, 1, bin);

    for (int i = 0; i < n; i++) {
        int m;
        scanf("%d", &m);
        
        CampoBusca criterios[m];
        int possuiCodEstacao, valorCodEstacao;
        
        // Leitura dos criterios de remocao (campo + valor) padronizada
        lerCriteriosBusca(criterios, m, &possuiCodEstacao, &valorCodEstacao);

        fseek(bin, TAM_CABECALHO, SEEK_SET);
        Registro r;
        int status_leitura;
        int rrn_atual = 0; // rastreia o RRN do registro em leitura para calcular o offset

        while ((status_leitura = leRegistroBin(bin, &r)) != 0) {
            if (status_leitura == 2) {
                rrn_atual++;
                continue; // pula removidos sem processar
            }

            // Checa match de remocao delegando para a funcao padrao
            if (registroSatisfazCriterios(&r, criterios, m)) {
                long offset_atual = TAM_CABECALHO + (rrn_atual * TAM_REGISTRO);

                // Empilha o registro: aponta seu 'proximo' para o atual topo e atualiza o topo
                r.removido = '1';
                r.proximo = c.topo;
                c.topo = rrn_atual;

                // Grava apenas os campos 'removido' e 'proximo' para nao sobrescrever dados desnecessariamente
                fseek(bin, offset_atual, SEEK_SET);
                fwrite(&r.removido, 1, 1, bin);
                fwrite(&r.proximo, 4, 1, bin);

                // Reposiciona o cursor para o inicio do proximo registro
                fseek(bin, offset_atual + TAM_REGISTRO, SEEK_SET);
            }

            rrn_atual++;
        }
    }

    // Reconta estacoes e pares unicos apos remocoes e restaura consistencia
    atualizaContagemEstacoes(bin, &c);
    atualizaCabecalho(bin, &c);
    fecharArquivo(bin);

    BinarioNaTela(arqBin);
}

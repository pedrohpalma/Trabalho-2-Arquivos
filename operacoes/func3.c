#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func3.h"
#include "../arquivos/arquivos.h"
#include "../cabecalho/cabecalho.h"
#include "../registro/registro.h"
#include "../utilitarios/utils.h"

// Funcao Select Where: executa 'n' buscas com multiplos criterios no arquivo binario
void func3(char *arqBin, int n) {
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

    for (int i = 0; i < n; i++) {
        int m;
        scanf("%d", &m);
        
        CampoBusca criterios[m];
        int possuiCodEstacao, valorCodEstacao;
        
        // Leitura dos m criterios de busca (campo + valor) centralizada na struct padrao
        lerCriteriosBusca(criterios, m, &possuiCodEstacao, &valorCodEstacao);

        // Reinicia a leitura a partir do primeiro registro de dados
        fseek(bin, TAM_CABECALHO, SEEK_SET);

        Registro r;
        int status_leitura;
        int encontrados = 0;

        // Varredura sequencial; interrompida antecipadamente se busca por codEstacao encontrou resultado
        while ((status_leitura = leRegistroBin(bin, &r)) != 0) {
            if (status_leitura == 2) continue; // pula registros logicamente removidos

            // Verifica se o registro satisfaz todos os m criterios simultaneamente usando a funcao utilitaria
            if (registroSatisfazCriterios(&r, criterios, m)) {
                encontrados++;
                imprimeRegistro(&r);

                // codEstacao e chave unica: nao ha necessidade de continuar percorrendo o arquivo
                if (possuiCodEstacao) {
                    break;
                }
            }
        }

        // Nenhum registro ativo satisfez os criterios fornecidos
        if (encontrados == 0) {
            printf("Registro inexistente.\n");
        }

        printf("\n");
    }

    fecharArquivo(bin);
}
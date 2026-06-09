#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "search.h"
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

        char campos[m][30];
        char valoresStr[m][100];
        int valoresInt[m];
        int isNulo[m]; // 1 se o usuario digitar "NULO"

        // Leitura dos m criterios de busca (campo + valor)
        for (int j = 0; j < m; j++) {
            scanf("%s", campos[j]);

            // Campos de tamanho variavel (strings) usam ScanQuoteString
            if (strcmp(campos[j], "nomeEstacao") == 0 || strcmp(campos[j], "nomeLinha") == 0) {
                ScanQuoteString(valoresStr[j]);
                isNulo[j] = (strcmp(valoresStr[j], "") == 0 || strcmp(valoresStr[j], "NULO") == 0) ? 1 : 0;
            }
            // Campos inteiros lidos como string e convertidos
            else {
                char temp[30];
                scanf("%s", temp);
                if (strcmp(temp, "NULO") == 0) {
                    isNulo[j] = 1;
                    valoresInt[j] = -1;
                } else {
                    isNulo[j] = 0;
                    valoresInt[j] = atoi(temp);
                }
            }
        }

        // Verifica se algum dos criterios e por codEstacao (campo chave unico).
        // Nesse caso, podemos interromper a busca apos encontrar o primeiro registro.
        int busca_por_codEstacao = 0;
        for (int j = 0; j < m; j++) {
            if (strcmp(campos[j], "codEstacao") == 0 && !isNulo[j]) {
                busca_por_codEstacao = 1;
                break;
            }
        }

        // Reinicia a leitura a partir do primeiro registro de dados
        fseek(bin, TAM_CABECALHO, SEEK_SET);

        Registro r;
        int status_leitura;
        int encontrados = 0;

        // Varredura sequencial; interrompida antecipadamente se busca por codEstacao encontrou resultado
        while ((status_leitura = leRegistroBin(bin, &r)) != 0) {
            if (status_leitura == 2) continue; // pula registros logicamente removidos

            int match = 1;

            // Verifica se o registro satisfaz todos os m criterios simultaneamente
            for (int j = 0; j < m; j++) {
                if (!atendeCriterio(&r, campos[j], valoresStr[j], valoresInt[j], isNulo[j])) {
                    match = 0;
                    break;
                }
            }

            if (match) {
                encontrados++;
                imprimeRegistro(&r);

                // codEstacao e chave unica: nao ha necessidade de continuar percorrendo o arquivo
                if (busca_por_codEstacao) {
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

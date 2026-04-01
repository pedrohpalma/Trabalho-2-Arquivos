#include <stdio.h>
#include <string.h>
#include "operacoes.h"
#include "../arquivos/arquivos.h"
#include "../cabecalho/cabecalho.h"
#include "../registro/registro.h"
#include "../utilitarios/utils.h"

void funcionalidade_1(char *arqEntrada, char *arqSaida) {
    FILE *csv = abrir_arquivo(arqEntrada, "r");
    if (!csv) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    FILE *bin = abrir_arquivo(arqSaida, "wb");
    if (!bin) {
        printf("Falha no processamento do arquivo.\n");
        fechar_arquivo(csv);
        return;
    }

    Cabecalho c;
    inicializa_cabecalho(&c);
    escreve_cabecalho(bin, &c);

    char linha[1024];
    // Ignorar cabeçalho do arquivo .csv
    fgets(linha, sizeof(linha), csv);

    // Arrays para validar unicidade (Tamanhos estimados para o cenário)
    char estacoes_unicas[2000][100];
    int pares_unicos[2000][2];
    
    while (fgets(linha, sizeof(linha), csv)) {
        Registro r;
        parse_linha_csv(linha, &r);
        escreve_registro_bin(bin, &r);
        
        // Verifica e contabiliza estações com nomes únicos
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

        // Verifica e contabiliza pares únicos (se existir próxima estação)
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

    // Atualiza cabeçalho com o status de sucesso '1' e os números finais 
    atualiza_cabecalho(bin, &c);

    fechar_arquivo(csv);
    fechar_arquivo(bin);

    // Usa a funcionalidade exigida
    BinarioNaTela(arqSaida);
}
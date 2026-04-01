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

void funcionalidade_2(char *arqBin) {
    FILE *bin = abrir_arquivo(arqBin, "rb");
    if (!bin) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    Cabecalho c;
    // Lê o status primeiro
    if (fread(&c.status, 1, 1, bin) != 1) {
        printf("Falha no processamento do arquivo.\n");
        fechar_arquivo(bin);
        return;
    }
    
    // Se o status for '0', o arquivo está inconsistente
    if (c.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fechar_arquivo(bin);
        return;
    }
    
    // Lê o restante do cabeçalho
    fread(&c.topo, 4, 1, bin);
    fread(&c.proxRRN, 4, 1, bin);
    fread(&c.nroEstacoes, 4, 1, bin);
    fread(&c.nroParesEstacao, 4, 1, bin);
    
    // Se não houver registros inseridos
    if (c.proxRRN == 0) {
        printf("Registro inexistente.\n");
        fechar_arquivo(bin);
        return;
    }

    int encontrados = 0;
    Registro r;
    
    // Itera pelos registros usando a nossa nova função de leitura
    while (le_registro_bin(bin, &r)) {
        // Ignora os registros marcados como removidos logicamente ('1')
        if (r.removido == '1') continue;
        
        encontrados++;
        
        // ========================================================
        // ÁREA DE FORMATAÇÃO DO PRINT
        // Ajuste aqui se o seu PDF exigir um layout visual diferente
        // Valores nulos numéricos (-1) e strings vazias (tamanho 0)
        // ========================================================
        
        printf("%d ", r.codEstacao);
        printf("%s ", r.nomeEstacao);
        
        if (r.codLinha == -1) printf("NULO ");
        else printf("%d ", r.codLinha);
        
        if (r.tamNomeLinha == 0) printf("NULO ");
        else printf("%s ", r.nomeLinha);
        
        if (r.codProxEstacao == -1) printf("NULO ");
        else printf("%d ", r.codProxEstacao);
        
        if (r.distProxEstacao == -1) printf("NULO ");
        else printf("%d ", r.distProxEstacao);
        
        if (r.codLinhaIntegra == -1) printf("NULO ");
        else printf("%d ", r.codLinhaIntegra);
        
        if (r.codEstIntegra == -1) printf("NULO\n");
        else printf("%d\n", r.codEstIntegra);
    }
    
    // Se todos os registros lidos estiverem deletados (pouco provável agora, mas é boa prática)
    if (encontrados == 0) {
        printf("Registro inexistente.\n");
    }

    fechar_arquivo(bin);
}
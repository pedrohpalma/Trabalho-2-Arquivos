#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "operacoes.h"
#include "../arquivos/arquivos.h"
#include "../cabecalho/cabecalho.h"
#include "../registro/registro.h"
#include "../utilitarios/utils.h"

// Funçao 'Create Table' que lê o .csv, converte para binário e escreve o cabeçalho
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

// Função 'Select * From' que lê o binário e imprime os registros não removidos
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
    int status_leitura;
    
    // Itera pelos registros usando a nossa nova função de leitura
    while ((status_leitura = le_registro_bin(bin, &r)) != 0){
        // Ignora os registros marcados como removidos logicamente ('1')
        if (status_leitura == 2) continue;
        
        encontrados++;
        imprime_registro(&r);
    }
    
    // Se todos os registros lidos estiverem deletados
    if (encontrados == 0) {
        printf("Registro inexistente.\n");
    }

    fechar_arquivo(bin);
}

// Função 'Select * From Where' com múltiplos critérios de busca
void funcionalidade_3(char *arqBin, int n) {
    FILE *bin = abrir_arquivo(arqBin, "rb");
    if (!bin) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    Cabecalho c;
    // Lê o status primeiro
    if (fread(&c.status, 1, 1, bin) != 1 || c.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fechar_arquivo(bin);
        return;
    }
    
    // Lê o restante do cabeçalho
    fread(&c.topo, 4, 1, bin);
    fread(&c.proxRRN, 4, 1, bin);
    fread(&c.nroEstacoes, 4, 1, bin);
    fread(&c.nroParesEstacao, 4, 1, bin);

    // Executa as buscas conforme os critérios fornecidos pelo usuario
    for (int i = 0; i < n; i++) {
        int m;
        scanf("%d", &m);

        char campos[m][30];
        char valoresStr[m][100];
        int valoresInt[m];
        int isNulo[m]; // 1 se o usuário digitar "NULO"

        // Leitura dos critérios de busca (campo e valor)
        for (int j = 0; j < m; j++) {
            scanf("%s", campos[j]);

            // Para campos variaveis (strings)
            if (strcmp(campos[j], "nomeEstacao") == 0 || strcmp(campos[j], "nomeLinha") == 0) {
                ScanQuoteString(valoresStr[j]);
                
                if (strcmp(valoresStr[j], "") == 0 || strcmp(valoresStr[j], "NULO") == 0) {
                    isNulo[j] = 1; 
                } else {
                    isNulo[j] = 0;
                }
            } 
            // Para campos fixos (inteiros)
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

        fseek(bin, 17, SEEK_SET); 
        
        Registro r;
        int status_leitura;
        int encontrados = 0;

        // Loop de leitura sequencial usando a sua funçao
        while ((status_leitura = le_registro_bin(bin, &r)) != 0) {

            if (status_leitura == 2) continue; 

            int match = 1;

            // Testa o registro contra todos os 'm' critérios
            for (int j = 0; j < m; j++) {
                if (!atende_criterio(&r, campos[j], valoresStr[j], valoresInt[j], isNulo[j])) {
                    match = 0;
                    break;
                }
            }

            // Se for 1, significa que ele atendeu a todos os 'm' filtros e é valido
            if (match) {
                encontrados++;
                imprime_registro(&r);
            }
        }

        // Se terminou a busca no arquivo e nao encontrou nenhum registro que atenda aos criterios
        if (encontrados == 0) {
            printf("Registro inexistente.\n");
        }

        // Imprime uma quebra de linha extra para separar as buscas, exceto após a última busca.
        if (i < n - 1) {
            printf("\n");
        }
    }

    fechar_arquivo(bin);
}

// Função 'Delete From' com múltiplos critérios de busca e marcação lógica de remoção
void funcionalidade_4(char *arqBin, int n) {
    // Abre em "r+b" para permitir leitura e alteração no mesmo arquivo
    FILE *bin = abrir_arquivo(arqBin, "r+b");
    if (!bin) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    Cabecalho c;
    if (fread(&c.status, 1, 1, bin) != 1 || c.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fechar_arquivo(bin);
        return;
    }
    
    // Marca o arquivo como inconsistente durante a operação
    c.status = '0';
    fseek(bin, 0, SEEK_SET);
    fwrite(&c.status, 1, 1, bin);
    
    // Lê o restante do cabeçalho para ter o valor de c.topo
    fread(&c.topo, 4, 1, bin);
    fread(&c.proxRRN, 4, 1, bin);
    fread(&c.nroEstacoes, 4, 1, bin);
    fread(&c.nroParesEstacao, 4, 1, bin);

    // Loop para as 'n' deleções
    for (int i = 0; i < n; i++) {
        int m;
        scanf("%d", &m);

        char campos[m][30];
        char valoresStr[m][100];
        int valoresInt[m];
        int isNulo[m];

        // Lógica de leitura de critérios
        for (int j = 0; j < m; j++) {
            scanf("%s", campos[j]);
            if (strcmp(campos[j], "nomeEstacao") == 0 || strcmp(campos[j], "nomeLinha") == 0) {
                ScanQuoteString(valoresStr[j]);
                isNulo[j] = (strcmp(valoresStr[j], "") == 0 || strcmp(valoresStr[j], "NULO") == 0) ? 1 : 0;
            } else {
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

        fseek(bin, 17, SEEK_SET); 
        
        Registro r;
        int status_leitura;
        
        // Guarda a posição exata onde o registro que vamos ler começa
        long offset_atual = ftell(bin); 

        while ((status_leitura = le_registro_bin(bin, &r)) != 0) {
            if (status_leitura == 2) {
                offset_atual = ftell(bin);
                continue; 
            }

            int match = 1;

            for (int j = 0; j < m; j++) {
                if (!atende_criterio(&r, campos[j], valoresStr[j], valoresInt[j], isNulo[j])) {
                    match = 0;
                    break;
                }
            }

            if (match) {
                r.removido = '1';
                r.proximo = c.topo;
                c.topo = (int)offset_atual;

                // Volta o ponteiro do HD para o início do registro que queremos marcar como removido
                fseek(bin, offset_atual, SEEK_SET);
                
                // Sobrescreve apenas os 5 bytes de controle (removido + proximo) para marcar como removido e atualizar o topo da lista de removidos
                fwrite(&r.removido, 1, 1, bin);
                fwrite(&r.proximo, 4, 1, bin);
                
                fseek(bin, offset_atual + 80, SEEK_SET);
            }
            
            // Atualiza o offset para o início do próximo registro
            offset_atual = ftell(bin); 
        }
    }

    // Depois das deleções, atualizamos o cabeçalho no arquivo para atualizar o status para '1' e o novo valor de topo
    atualiza_cabecalho(bin, &c);
    fechar_arquivo(bin);

    BinarioNaTela(arqBin);
}
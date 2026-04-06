#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "operacoes.h"
#include "../arquivos/arquivos.h"
#include "../cabecalho/cabecalho.h"
#include "../registro/registro.h"
#include "../utilitarios/utils.h"



// Funçao Create Table que le o .csv, converte para binário e escreve o cabeçalho
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
    escreveCabecalho(bin, &c); //inicia cabeçalho zerado

    char linha[1024];
    fgets(linha, sizeof(linha), csv);// Ignorar cabeçalho do arquivo .csv

    // Arrays para validar unicidade (Tamanhos estimados para o cenário)
    char estacoes_unicas[2000][100];
    int pares_unicos[2000][2];
    
    while (fgets(linha, sizeof(linha), csv)) {
        Registro r;
        lerRegistroCSV(linha, &r);
        escreveRegistroBin(bin, &r);
        
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



    // atualiza cabeçalho com o status de sucesso '1' e os valores finais 
    atualizaCabecalho(bin, &c);

    fecharArquivo(csv);
    fecharArquivo(bin);

    BinarioNaTela(arqSaida);
}



// Função 'Select * From' que lê o binário e imprime os registros não removidos
void func2(char *arqBin) {
    FILE *bin = abrirArquivo(arqBin, "rb");
    if (!bin) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    Cabecalho c;
    
    // Verifica a consistência do arquivo e lê o cabeçalho
    if (!leCabecalho(bin, &c)) {
            printf("Falha no processamento do arquivo.\n");
            fecharArquivo(bin);
            return;
        }
    
    // Se não houver registros inseridos
    if (c.proxRRN == 0) {
        printf("Registro inexistente.\n");
        fecharArquivo(bin);
        return;
    }

    int encontrados = 0;
    Registro r;
    int status_leitura;
    
    // Itera pelos registros usando a nossa nova função de leitura
    while ((status_leitura = leRegistroBin(bin, &r)) != 0){
        if (status_leitura == 2) continue;// Ignora os registros marcados como removidos logicamente
        
        encontrados++;
        imprimeRegistro(&r);
    }
    
    // Se todos os registros lidos estiverem deletados
    if (encontrados == 0) {
        printf("Registro inexistente.\n");
    }

    fecharArquivo(bin);
}



// Função 'Select * From Where' com múltiplos critérios de busca
void func3(char *arqBin, int n) {
    FILE *bin = abrirArquivo(arqBin, "rb");
    if (!bin) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    Cabecalho c;

    // Verifica a consistência do arquivo e lê o cabeçalho
    if (!leCabecalho(bin, &c)) {
            printf("Falha no processamento do arquivo.\n");
            fecharArquivo(bin);
            return;
        }

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

            //para campos de tamanho variaveis (strings)
            if (strcmp(campos[j], "nomeEstacao") == 0 || strcmp(campos[j], "nomeLinha") == 0) {
                ScanQuoteString(valoresStr[j]);
                
                if (strcmp(valoresStr[j], "") == 0 || strcmp(valoresStr[j], "NULO") == 0) {
                    isNulo[j] = 1; 
                } else {
                    isNulo[j] = 0;
                }
            } 
            // Para campos com valores inteiros
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

        fseek(bin, TAM_CABECALHO, SEEK_SET); 
        
        Registro r;
        int status_leitura;
        int encontrados = 0;

        // Loop de leitura sequencial usando a funçao leRegistroBin
        while ((status_leitura = leRegistroBin(bin, &r)) != 0) {

            if (status_leitura == 2) continue; //registro removido logicamente

            int match = 1;

            // Testa o registro contra todos os m criterios
            for (int j = 0; j < m; j++) {
                if (!atendeCriterio(&r, campos[j], valoresStr[j], valoresInt[j], isNulo[j])) {
                    match = 0;
                    break;
                }
            }

            // Se for 1 significa que ele atendeu a todos os m filtros e é valido
            if (match) {
                encontrados++;
                imprimeRegistro(&r);
            }
        }

        // Se terminou a busca no arquivo e nao encontrou nenhum registro que atenda aos criterios
        if (encontrados == 0) {
            printf("Registro inexistente.\n");
        }

        printf("\n");

    }

    fecharArquivo(bin);
}



// Função 'Delete From' com remoção lógica
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
    
    // Marca o arquivo como inconsistente durante a operação
    c.status = '0';
    fseek(bin, 0, SEEK_SET);
    fwrite(&c.status, 1, 1, bin);

    // Loop para as 'n' deleções
    for (int i = 0; i < n; i++) {
        int m;
        scanf("%d", &m);

        char campos[m][30];
        char valoresStr[m][100];
        int valoresInt[m];
        int isNulo[m];


        //lógica de leitura dos critérios
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

        fseek(bin, TAM_CABECALHO, SEEK_SET); 
        Registro r;
        int status_leitura;
        int rrn_atual = 0; 

        while ((status_leitura = leRegistroBin(bin, &r)) != 0) {
            if (status_leitura == 2) {
                rrn_atual++;
                continue; 
            }

            int match = 1;

            for (int j = 0; j < m; j++) {
                if (!atendeCriterio(&r, campos[j], valoresStr[j], valoresInt[j], isNulo[j])) {
                    match = 0;
                    break;
                }
            }

            if (match) {
                long offset_atual = TAM_CABECALHO + (rrn_atual * TAM_REGISTRO); //acha o offset do registro a ser removido


                //marca como removido e atualiza o topo da pilha
                r.removido = '1';
                r.proximo = c.topo; 
                c.topo = rrn_atual; 


                //passa mudanças pro arquivo binario
                fseek(bin, offset_atual, SEEK_SET);
                fwrite(&r.removido, 1, 1, bin);
                fwrite(&r.proximo, 4, 1, bin); 
                
                fseek(bin, offset_atual + TAM_REGISTRO, SEEK_SET);
            }
            
            rrn_atual++; 
        }
    }


    //atualiza contagem de estacoes e pares de estacoes
    atualizaContagemEstacoes(bin, &c);

    atualizaCabecalho(bin, &c);
    fecharArquivo(bin);

    BinarioNaTela(arqBin);
}



//Função Insert Into, permite a inserção de novos registros, sobrescrevendo registros removidos logicamente quando necessário
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

    // Muda o status para inconsistente enquanto processa inserções
    c.status = '0';
    fseek(bin, 0, SEEK_SET);
    fwrite(&c.status, 1, 1, bin);
    fflush(bin); 

    //loop das buscas 
    for (int i = 0; i < n; i++) {
        Registro r;
        leRegistroTeclado(&r); //le entrada do usuário para registro

        // verificação de reuso de espaço
        if (c.topo != -1) { 
            int rrn_reuso = c.topo;
            long offset = TAM_CABECALHO + (rrn_reuso * TAM_REGISTRO); //ofset do prox registro logicamente removido

            int proximo_removido;
            fseek(bin, offset + 1, SEEK_SET); 
            fread(&proximo_removido, 4, 1, bin);

            c.topo = proximo_removido; //remove um removido da pilha
            fseek(bin, offset, SEEK_SET);
            escreveRegistroBin(bin, &r); 
        } else {
            long offset = TAM_CABECALHO + (c.proxRRN * TAM_REGISTRO);
            fseek(bin, offset, SEEK_SET);
            escreveRegistroBin(bin, &r);  //escreve registro novo no offset determinado pela verificação
            c.proxRRN++;
        }
    }

    atualizaContagemEstacoes(bin, &c); //atualiza contagem de estacoes e pares. PDF falava que nao precisava, mas no runcodes precisava

    atualizaCabecalho(bin, &c);
    fecharArquivo(bin);

    BinarioNaTela(arqBin);
}


//Função Update: atualiza registros conforme os campos especificados
void func6(char *arqBin, int n) {
    FILE *bin = abrirArquivo(arqBin, "rb+");
    if (!bin) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    Cabecalho c;
    // Verifica a consistência do arquivo e lê o cabeçalho
    if (!leCabecalho(bin, &c)) {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        return;
    }

    c.status = '0'; //atualiza consistencia do cabeçalho
    fseek(bin, 0, SEEK_SET);
    fwrite(&c.status, 1, 1, bin);
    fflush(bin); 

    for (int i = 0; i < n; i++) { //leitura dos critérios de busca
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


        //leitura dos critérios de atualização
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

        //Rewind força a limpeza da flag de EOF antes do fseek
        rewind(bin);
        fseek(bin, TAM_CABECALHO, SEEK_SET);

        Registro r;
        int status_leitura;
        int rrn_atual = 0;

        while ((status_leitura = leRegistroBin(bin, &r)) != 0) {
            if (status_leitura == 2) {
                rrn_atual++; // Avança o RRN mesmo se o registro for removido
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
                for (int j = 0; j < p; j++) {
                    autualizaCampo(&r, camposAtualiza[j], valoresStrAtualiza[j], valoresIntAtualiza[j], isNuloAtualiza[j]);
                }

                long offset_atual = TAM_CABECALHO + (rrn_atual * TAM_REGISTRO);
                
                fseek(bin, offset_atual, SEEK_SET);
                escreveRegistroBin(bin, &r);
                fflush(bin); 
                
                // Reposiciona no início do PRÓXIMO registro para manter a consistência do while
                fseek(bin, offset_atual + TAM_REGISTRO, SEEK_SET); 
            }
            rrn_atual++; // Avança o RRN após ler/processar um registro válido
        }
    }

    // Finaliza as atualizações e restaura consistência ('1')
    atualizaCabecalho(bin, &c);
    
    // Garante que o buffer final foi despejado
    fflush(bin); 
    fecharArquivo(bin);

    BinarioNaTela(arqBin);
}
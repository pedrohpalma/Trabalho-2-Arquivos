#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func8.h"
#include "../arquivos/arquivos.h"
#include "../cabecalho/cabecalho.h"
#include "../registro/registro.h"
#include "../utilitarios/utils.h"
#include "../arvoreB/arvoreB.h"
#include "../cabecalhoArvoreB/cabecalhoArvoreB.h"

// Funcao Auxiliar: Realiza a busca sequencial tradicional (fallback)
// Acionada quando o criterio de busca NAO inclui a chave primaria (codEstacao) indexada na Arvore-B.
static int buscarSequencialFunc8(FILE *bin, CampoBusca criterios[], int qtdCriterios){
    Registro r;
    int status_leitura;
    int encontrados = 0;

    fseek(bin, TAM_CABECALHO, SEEK_SET);

    while ((status_leitura = leRegistroBin(bin, &r)) != 0){
        if (status_leitura == 2) {
            continue; // Pula registros removidos logicamente
        }

        // Utiliza a funcao padronizada do registro.h para checar os criterios
        if (registroSatisfazCriterios(&r, criterios, qtdCriterios)){
            imprimeRegistro(&r);
            encontrados = 1;
        }
    }
    return encontrados;
}

// Funcao Auxiliar: Realiza a busca otimizada utilizando a Arvore-B
// Acionada quando o usuario fornece o 'codEstacao'. Busca o offset no indice em O(log n) e le direto do binario.
static int buscarComIndiceFunc8(FILE *bin, FILE *indice, CampoBusca criterios[], int qtdCriterios, int valorCodEstacao){
    int referencia; // Armazenara o byteOffset retornado pela Arvore-B
    Registro r;
    int status_leitura;

    // Busca a chave na Arvore-B. Se nao encontrar, o registro nao existe.
    if (!buscarArvoreB(indice, valorCodEstacao, &referencia)){
        return 0;
    }

    // Salta diretamente para o byteOffset retornado pelo indice
    fseek(bin, referencia, SEEK_SET);
    status_leitura = leRegistroBin(bin, &r);

    // Garante que a leitura foi bem sucedida (nao deve ser EOF nem removido, pois o indice so guarda registros ativos)
    if (status_leitura != 1){
        return 0;
    }

    // Como pode haver OUTROS criterios alem do codEstacao (ex: codEstacao = 10 E nomeLinha = "Azul"),
    // validamos se o registro encontrado atende a todos os outros filtros.
    if (!registroSatisfazCriterios(&r, criterios, qtdCriterios)){
        return 0;
    }

    imprimeRegistro(&r);
    return 1;
}

// Funcao Select Where com Indice: Direciona a busca para o metodo sequencial ou indexado
void func8(char *arqEntrada, char *arqIndice, int n){
    FILE *bin = abrirArquivo(arqEntrada, "rb");
    if (!bin){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    FILE *indice = abrirArquivo(arqIndice, "rb");
    if (!indice){
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        return;
    }

    Cabecalho c;
    if (!leCabecalho(bin, &c)){
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        fecharArquivo(indice);
        return;
    }

    CabecalhoArvoreB cabecalhoIndice;
    // Verifica consistencia de AMBOS os arquivos antes de operar
    if (!lerCabecalhoArvoreB(indice, &cabecalhoIndice) || cabecalhoIndice.status != '1'){
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        fecharArquivo(indice);
        return;
    }

    for (int i = 0; i < n; i++){
        int m;
        scanf("%d", &m);

        CampoBusca criterios[m];
        int possuiCodEstacao, valorCodEstacao, encontrados;

        // Le os criterios e identifica rapidamente se a chave primaria (codEstacao) esta entre eles
        lerCriteriosBusca(criterios, m, &possuiCodEstacao, &valorCodEstacao);

        // Roteamento inteligente: usa Arvore-B se tiver a chave, senao varre sequencialmente
        if (possuiCodEstacao){
            encontrados = buscarComIndiceFunc8(bin, indice, criterios, m, valorCodEstacao);
        } else{
            encontrados = buscarSequencialFunc8(bin, criterios, m);
        }

        if (!encontrados){
            printf("Registro inexistente.\n");
        }
        printf("\n");
    }

    fecharArquivo(bin);
    fecharArquivo(indice);
}
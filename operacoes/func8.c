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

// busca sequencial quando a busca não inclui o campo codEstacao
static int buscarSequencialFunc8(FILE *bin, CampoBusca criterios[], int qtdCriterios)
{
    Registro r;
    int status_leitura;
    int encontrados = 0;

    fseek(bin, TAM_CABECALHO, SEEK_SET);

    while ((status_leitura = leRegistroBin(bin, &r)) != 0)
    {
        if (status_leitura == 2)
        {
            continue; // Pula registros removidos logicamente
        }

        // utiliza a funcao padronizada do registro.h para checar os criterios
        if (registroSatisfazCriterios(&r, criterios, qtdCriterios))
        {
            imprimeRegistro(&r);
            encontrados = 1;
        }
    }
    return encontrados;
}

// realiza a busca otimizada utilizando a arvoreB quando codEstacao é um dos critérios
static int buscarComIndiceFunc8(FILE *bin, FILE *indice, CampoBusca criterios[], int qtdCriterios, int valorCodEstacao)
{
    int referencia; // Armazenara o byteOffset retornado pela Arvore-B
    Registro r;
    int status_leitura;

    // busca a chave na arvoreB. se nao encontrar o registro nao existe.
    if (!buscarArvoreB(indice, valorCodEstacao, &referencia))
    {
        return 0;
    }

    // salta diretamente para o byteOffset retornado pelo indice
    fseek(bin, referencia, SEEK_SET);
    status_leitura = leRegistroBin(bin, &r);

    // garante que a leitura foi bem sucedida (nao deve ser EOF nem removido, pois o indice so guarda registros ativos)
    if (status_leitura != 1)
    {
        return 0;
    }

    // validaçao com os outros criterios fora codEstacao
    if (!registroSatisfazCriterios(&r, criterios, qtdCriterios))
    {
        return 0;
    }

    imprimeRegistro(&r);
    return 1;
}

// busca que decide se vai ser sequencial ou indexada com base nos campos de busca
void func8(char *arqEntrada, char *arqIndice, int n)
{
    FILE *bin = abrirArquivo(arqEntrada, "rb");
    if (!bin)
    {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    FILE *indice = abrirArquivo(arqIndice, "rb");
    if (!indice)
    {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        return;
    }

    Cabecalho c;
    if (!leCabecalho(bin, &c))
    {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        fecharArquivo(indice);
        return;
    }

    CabecalhoArvoreB cabecalhoIndice;
    // verifica consistencia dos arquivos antes de operar
    if (!lerCabecalhoArvoreB(indice, &cabecalhoIndice) || cabecalhoIndice.status != '1')
    {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        fecharArquivo(indice);
        return;
    }

    for (int i = 0; i < n; i++)
    {
        int m;
        scanf("%d", &m);

        CampoBusca criterios[m];
        int possuiCodEstacao, valorCodEstacao, encontrados;

        // Le os criterios e identifica  se codEstacao esta entre eles
        lerCriteriosBusca(criterios, m, &possuiCodEstacao, &valorCodEstacao);

        // usa Arvore-B se tiver a chave, senao varre sequencialmente
        if (possuiCodEstacao)
        {
            encontrados = buscarComIndiceFunc8(bin, indice, criterios, m, valorCodEstacao);
        }
        else
        {
            encontrados = buscarSequencialFunc8(bin, criterios, m);
        }

        if (!encontrados)
        {
            printf("Registro inexistente.\n");
        }
        printf("\n");
    }

    fecharArquivo(bin);
    fecharArquivo(indice);
}
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

typedef struct
{
    char nomeCampo[50];
    int valorInt;
    char valorStr[100];
    int isNulo;
} CampoBusca;

static int campoString(char *campo)
{
    return strcmp(campo, "nomeEstacao") == 0 || strcmp(campo, "nomeLinha") == 0;
}

static void lerCriteriosBusca(CampoBusca criterios[], int qtdCriterios,
                              int *possuiCodEstacao, int *valorCodEstacao)
{
    *possuiCodEstacao = 0;
    *valorCodEstacao = -1;

    for (int i = 0; i < qtdCriterios; i++)
    {
        scanf("%s", criterios[i].nomeCampo);
        criterios[i].valorInt = -1;
        criterios[i].valorStr[0] = '\0';
        criterios[i].isNulo = 0;

        if (campoString(criterios[i].nomeCampo))
        {
            ScanQuoteString(criterios[i].valorStr);
            criterios[i].isNulo = (strcmp(criterios[i].valorStr, "") == 0 ||
                                   strcmp(criterios[i].valorStr, "NULO") == 0)
                                      ? 1
                                      : 0;
        }
        else
        {
            char temp[50];
            scanf("%s", temp);
            if (strcmp(temp, "NULO") == 0)
            {
                criterios[i].isNulo = 1;
                criterios[i].valorInt = -1;
            }
            else
            {
                criterios[i].valorInt = atoi(temp);
            }
        }

        if (strcmp(criterios[i].nomeCampo, "codEstacao") == 0)
        {
            *possuiCodEstacao = 1;
            *valorCodEstacao = criterios[i].valorInt;
        }
    }
}

static int registroSatisfazCriterios(Registro *r, CampoBusca criterios[], int qtdCriterios)
{
    if (r->removido == '1')
    {
        return 0;
    }

    for (int i = 0; i < qtdCriterios; i++)
    {
        if (!atendeCriterio(r, criterios[i].nomeCampo, criterios[i].valorStr,
                            criterios[i].valorInt, criterios[i].isNulo))
        {
            return 0;
        }
    }

    return 1;
}

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
            continue;
        }

        if (registroSatisfazCriterios(&r, criterios, qtdCriterios))
        {
            imprimeRegistro(&r);
            encontrados = 1;
        }
    }

    return encontrados;
}

static int buscarComIndiceFunc8(FILE *bin, FILE *indice, CampoBusca criterios[],
                                int qtdCriterios, int valorCodEstacao)
{
    int referencia;
    Registro r;
    int status_leitura;

    if (!buscarArvoreB(indice, valorCodEstacao, &referencia))
    {
        return 0;
    }

    fseek(bin, referencia, SEEK_SET);
    status_leitura = leRegistroBin(bin, &r);

    if (status_leitura != 1)
    {
        return 0;
    }

    if (!registroSatisfazCriterios(&r, criterios, qtdCriterios))
    {
        return 0;
    }

    imprimeRegistro(&r);
    return 1;
}

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
        int possuiCodEstacao;
        int valorCodEstacao;
        int encontrados;

        lerCriteriosBusca(criterios, m, &possuiCodEstacao, &valorCodEstacao);

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

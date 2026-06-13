#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func10.h"
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
} CampoBusca10;

typedef struct
{
    long byteOffset;
    int codEstacao;
} RegistroRemocao;

static int campoStringFunc10(char *campo)
{
    return strcmp(campo, "nomeEstacao") == 0 || strcmp(campo, "nomeLinha") == 0;
}

static void lerCriteriosFunc10(CampoBusca10 criterios[], int qtdCriterios,
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

        if (campoStringFunc10(criterios[i].nomeCampo))
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

static int registroSatisfazCriteriosFunc10(Registro *r, CampoBusca10 criterios[], int qtdCriterios)
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

static int adicionarRemocao(RegistroRemocao **remocoes, int *qtdRemocoes, int *capacidade,
                            long byteOffset, int codEstacao)
{
    if (*qtdRemocoes >= *capacidade)
    {
        int novaCapacidade = (*capacidade == 0) ? 8 : (*capacidade * 2);
        RegistroRemocao *novo = realloc(*remocoes, novaCapacidade * sizeof(RegistroRemocao));
        if (!novo)
        {
            return 0;
        }
        *remocoes = novo;
        *capacidade = novaCapacidade;
    }

    (*remocoes)[*qtdRemocoes].byteOffset = byteOffset;
    (*remocoes)[*qtdRemocoes].codEstacao = codEstacao;
    (*qtdRemocoes)++;
    return 1;
}

static int buscarRegistrosParaRemocao(FILE *bin, FILE *indice, CampoBusca10 criterios[],
                                      int qtdCriterios, int possuiCodEstacao, int valorCodEstacao,
                                      RegistroRemocao **remocoes, int *qtdRemocoes)
{
    int capacidade = 0;
    *remocoes = NULL;
    *qtdRemocoes = 0;

    if (possuiCodEstacao)
    {
        int referencia;
        Registro r;
        int status_leitura;

        if (!buscarArvoreB(indice, valorCodEstacao, &referencia))
        {
            return 1;
        }

        fseek(bin, referencia, SEEK_SET);
        status_leitura = leRegistroBin(bin, &r);

        if (status_leitura == 1 && registroSatisfazCriteriosFunc10(&r, criterios, qtdCriterios))
        {
            if (!adicionarRemocao(remocoes, qtdRemocoes, &capacidade, referencia, r.codEstacao))
                return 0;
        }

        return 1;
    }

    fseek(bin, TAM_CABECALHO, SEEK_SET);

    Registro r;
    int status_leitura;

    while (1)
    {
        long offsetRegistro = ftell(bin);
        status_leitura = leRegistroBin(bin, &r);

        if (status_leitura == 0)
        {
            break;
        }

        if (status_leitura == 2)
        {
            continue;
        }

        if (registroSatisfazCriteriosFunc10(&r, criterios, qtdCriterios))
        {
            if (!adicionarRemocao(remocoes, qtdRemocoes, &capacidade, offsetRegistro, r.codEstacao))
                return 0;
        }
    }

    return 1;
}

static int removerLogicamenteRegistroDados(FILE *bin, Cabecalho *c, long byteOffset)
{
    char removido;
    int rrn = (byteOffset - TAM_CABECALHO) / TAM_REGISTRO;
    int topoAntigo = c->topo;

    fseek(bin, byteOffset, SEEK_SET);
    if (fread(&removido, 1, 1, bin) != 1)
    {
        return 0;
    }

    if (removido == '1')
    {
        return 1;
    }

    removido = '1';
    fseek(bin, byteOffset, SEEK_SET);
    if (fwrite(&removido, 1, 1, bin) != 1)
    {
        return 0;
    }
    if (fwrite(&topoAntigo, 4, 1, bin) != 1)
    {
        return 0;
    }

    c->topo = rrn;
    fseek(bin, 0, SEEK_SET);
    escreveCabecalho(bin, c);
    fflush(bin);

    return 1;
}

void func10(char *arqEntrada, char *arqIndice, int n)
{
    FILE *bin = abrirArquivo(arqEntrada, "rb+");
    if (!bin)
    {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    FILE *indice = abrirArquivo(arqIndice, "rb+");
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

    c.status = '0';
    fseek(bin, 0, SEEK_SET);
    escreveCabecalho(bin, &c);

    cabecalhoIndice.status = '0';
    if (!escreverCabecalhoArvoreB(indice, &cabecalhoIndice))
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

        CampoBusca10 criterios[m];
        int possuiCodEstacao;
        int valorCodEstacao;
        RegistroRemocao *remocoes = NULL;
        int qtdRemocoes = 0;

        lerCriteriosFunc10(criterios, m, &possuiCodEstacao, &valorCodEstacao);

        if (!buscarRegistrosParaRemocao(bin, indice, criterios, m, possuiCodEstacao, valorCodEstacao,
                                        &remocoes, &qtdRemocoes))
        {
            free(remocoes);
            printf("Falha no processamento do arquivo.\n");
            fecharArquivo(bin);
            fecharArquivo(indice);
            return;
        }

        for (int j = 0; j < qtdRemocoes; j++)
        {
            if (!removerLogicamenteRegistroDados(bin, &c, remocoes[j].byteOffset))
            {
                free(remocoes);
                printf("Falha no processamento do arquivo.\n");
                fecharArquivo(bin);
                fecharArquivo(indice);
                return;
            }

            if (!removerArvoreB(indice, remocoes[j].codEstacao))
            {
                free(remocoes);
                printf("Falha no processamento do arquivo.\n");
                fecharArquivo(bin);
                fecharArquivo(indice);
                return;
            }
        }

        free(remocoes);
    }

    atualizaContagemEstacoes(bin, &c);
    atualizaCabecalho(bin, &c);

    if (!lerCabecalhoArvoreB(indice, &cabecalhoIndice))
    {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        fecharArquivo(indice);
        return;
    }
    cabecalhoIndice.status = '1';
    if (!escreverCabecalhoArvoreB(indice, &cabecalhoIndice))
    {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        fecharArquivo(indice);
        return;
    }

    fecharArquivo(bin);
    fecharArquivo(indice);

    BinarioNaTela(arqEntrada);
    BinarioNaTela(arqIndice);
}

#include <stdio.h>
#include "func7.h"
#include "../arquivos/arquivos.h"
#include "../cabecalho/cabecalho.h"
#include "../registro/registro.h"
#include "../utilitarios/utils.h"
#include "../arvoreB/arvoreB.h"
#include "../cabecalhoArvoreB/cabecalhoArvoreB.h"

void func7(char *arqEntrada, char *arqIndice)
{
    FILE *bin = abrirArquivo(arqEntrada, "rb"); // abertura do arquivo e verificaçao de processamento
    if (!bin)
    {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    Cabecalho c;
    if (!leCabecalho(bin, &c))
    {
        printf("Falha no processamento do arquivo.\n"); // cria cabeçalho do arquivo de dados e carrega ele
        fecharArquivo(bin);
        return;
    }

    FILE *indice = abrirArquivo(arqIndice, "wb+"); // cria arquivo de indices e verifica
    if (!indice)
    {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        return;
    }

    if (!criarArquivoIndiceArvoreB(indice))
    { // cria a árvore B vazia
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        fecharArquivo(indice);
        return;
    }

    fseek(bin, TAM_CABECALHO, SEEK_SET); // pula cabeçalho

    Registro r;
    int status_leitura;

    while (1)
    {
        long offsetRegistro = ftell(bin);
        status_leitura = leRegistroBin(bin, &r); // le registro e salva em r

        if (status_leitura == 0) // para se chegar em EOF
        {
            break;
        }

        if (status_leitura == 2)
        {
            continue;
        }

        if (!inserirArvoreB(indice, r.codEstacao, (int)offsetRegistro)) // insere na árvore B o registro retirado dos dados
        {
            printf("Falha no processamento do arquivo.\n");
            fecharArquivo(bin);
            fecharArquivo(indice);
            return;
        }
    }

    if (!atualizarStatusArvoreB(indice, '1')) // atualiza status para ocnsistente
    {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        fecharArquivo(indice);
        return;
    }

    fecharArquivo(bin);
    fecharArquivo(indice);

    BinarioNaTela(arqIndice);
}

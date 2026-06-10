#include "cabecalhoArvoreB.h"

// Cria o cabecalho inicial do arquivo de indice arvore-B
CabecalhoArvoreB criarCabecalhoArvoreBVazio()
{
    CabecalhoArvoreB cabecalho;
    cabecalho.status = '0';
    cabecalho.noRaiz = -1;
    cabecalho.topo = -1;
    cabecalho.proxRRN = 0;
    cabecalho.nroNos = 0;
    return cabecalho;
}

// Le os 17 bytes do cabecalho do arquivo de indice
int lerCabecalhoArvoreB(FILE *arquivoIndice, CabecalhoArvoreB *cabecalho)
{
    fseek(arquivoIndice, 0, SEEK_SET);

    if (fread(&cabecalho->status, 1, 1, arquivoIndice) != 1)
        return 0;
    if (fread(&cabecalho->noRaiz, 4, 1, arquivoIndice) != 1)
        return 0;
    if (fread(&cabecalho->topo, 4, 1, arquivoIndice) != 1)
        return 0;
    if (fread(&cabecalho->proxRRN, 4, 1, arquivoIndice) != 1)
        return 0;
    if (fread(&cabecalho->nroNos, 4, 1, arquivoIndice) != 1)
        return 0;

    return 1;
}

// Escreve os campos do cabecalho separadamente para manter o formato exato
int escreverCabecalhoArvoreB(FILE *arquivoIndice, CabecalhoArvoreB *cabecalho)
{
    fseek(arquivoIndice, 0, SEEK_SET);

    if (fwrite(&cabecalho->status, 1, 1, arquivoIndice) != 1)
        return 0;
    if (fwrite(&cabecalho->noRaiz, 4, 1, arquivoIndice) != 1)
        return 0;
    if (fwrite(&cabecalho->topo, 4, 1, arquivoIndice) != 1)
        return 0;
    if (fwrite(&cabecalho->proxRRN, 4, 1, arquivoIndice) != 1)
        return 0;
    if (fwrite(&cabecalho->nroNos, 4, 1, arquivoIndice) != 1)
        return 0;

    fflush(arquivoIndice);
    return 1;
}

// Atualiza apenas o byte de status do arquivo de indice
int atualizarStatusArvoreB(FILE *arquivoIndice, char status)
{
    fseek(arquivoIndice, 0, SEEK_SET);
    if (fwrite(&status, 1, 1, arquivoIndice) != 1)
        return 0;
    fflush(arquivoIndice);
    return 1;
}
#include "cabecalhoArvoreB.h"

// Inicializa e retorna uma estrutura de cabeçalho na memória de uma Árvore-B vazia.
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

// Lê os campos do cabeçalho do arquivo de índice e os armazena na estrutura fornecida. 
int lerCabecalhoArvoreB(FILE *arquivoIndice, CabecalhoArvoreB *cabecalho)
{
    // Lê os campos do cabeçalho separadamente para manter o formato exato
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

// Escreve os campos do cabeçalho de volta no arquivo de índice. 
int escreverCabecalhoArvoreB(FILE *arquivoIndice, CabecalhoArvoreB *cabecalho)
{
    // Move o ponteiro do arquivo para o início para escrever o cabeçalho
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

// Atualiza o campo de status do cabeçalho no arquivo de índice.
int atualizarStatusArvoreB(FILE *arquivoIndice, char status)
{
    fseek(arquivoIndice, 0, SEEK_SET);

    if (fwrite(&status, 1, 1, arquivoIndice) != 1)
        return 0;
    fflush(arquivoIndice);
    return 1;
}
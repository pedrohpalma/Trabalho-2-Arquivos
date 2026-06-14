#include "noArvoreB.h"
#include "../cabecalhoArvoreB/cabecalhoArvoreB.h"

// Preenche todos os campos nao utilizados com -1
void limparNoAtivo(NoArvoreB *no)
{
    for (int i = no->nroChaves; i < MAX_CHAVES_ARVORE_B; i++)
    {
        no->C[i] = -1;
        no->PR[i] = -1;
    }

    for (int i = no->nroChaves + 1; i < ORDEM_ARVORE_B; i++)
    {
        no->P[i] = -1;
    }

    if (no->tipoNo == -1)
    {
        for (int i = 0; i < ORDEM_ARVORE_B; i++)
        {
            no->P[i] = -1;
        }
    }
}

// Cria um no novo, ativo, sem chaves e sem filhos
NoArvoreB criarNoArvoreBVazio(int tipoNo)
{
    NoArvoreB no;
    no.removido = '0';
    no.proximo = -1;
    no.tipoNo = tipoNo;
    no.nroChaves = 0;

    for (int i = 0; i < MAX_CHAVES_ARVORE_B; i++)
    {
        no.C[i] = -1;
        no.PR[i] = -1;
    }

    for (int i = 0; i < ORDEM_ARVORE_B; i++)
    {
        no.P[i] = -1;
    }

    return no;
}

// Converte RRN de no para byte offset no arquivo de indice
long calcularOffsetNoArvoreB(int rrn)
{
    return TAM_CABECALHO_ARVORE_B + (rrn * TAM_NO_ARVORE_B);
}

// Le um no da arvore-B campo a campo
int lerNoArvoreB(FILE *arquivoIndice, int rrn, NoArvoreB *no)
{
    fseek(arquivoIndice, calcularOffsetNoArvoreB(rrn), SEEK_SET);

    if (fread(&no->removido, 1, 1, arquivoIndice) != 1)
        return 0;
    if (fread(&no->proximo, 4, 1, arquivoIndice) != 1)
        return 0;
    if (fread(&no->tipoNo, 4, 1, arquivoIndice) != 1)
        return 0;
    if (fread(&no->nroChaves, 4, 1, arquivoIndice) != 1)
        return 0;

    for (int i = 0; i < MAX_CHAVES_ARVORE_B; i++)
    {
        if (fread(&no->C[i], 4, 1, arquivoIndice) != 1)
            return 0;
        if (fread(&no->PR[i], 4, 1, arquivoIndice) != 1)
            return 0;
    }

    for (int i = 0; i < ORDEM_ARVORE_B; i++)
    {
        if (fread(&no->P[i], 4, 1, arquivoIndice) != 1)
            return 0;
    }

    return 1;
}

// Escreve um no da arvore-B campo a campo, garantindo 53 bytes
int escreverNoArvoreB(FILE *arquivoIndice, int rrn, NoArvoreB *no)
{
    limparNoAtivo(no);
    fseek(arquivoIndice, calcularOffsetNoArvoreB(rrn), SEEK_SET);

    if (fwrite(&no->removido, 1, 1, arquivoIndice) != 1)
        return 0;
    if (fwrite(&no->proximo, 4, 1, arquivoIndice) != 1)
        return 0;
    if (fwrite(&no->tipoNo, 4, 1, arquivoIndice) != 1)
        return 0;
    if (fwrite(&no->nroChaves, 4, 1, arquivoIndice) != 1)
        return 0;

    for (int i = 0; i < MAX_CHAVES_ARVORE_B; i++)
    {
        if (fwrite(&no->C[i], 4, 1, arquivoIndice) != 1)
            return 0;
        if (fwrite(&no->PR[i], 4, 1, arquivoIndice) != 1)
            return 0;
    }

    for (int i = 0; i < ORDEM_ARVORE_B; i++)
    {
        if (fwrite(&no->P[i], 4, 1, arquivoIndice) != 1)
            return 0;
    }

    fflush(arquivoIndice);
    return 1;
}

// Retorna a posicao em que a chave deve ser buscada ou inserida no no
int buscarPosicaoNo(NoArvoreB *no, int chave)
{
    int pos = 0;

    while (pos < no->nroChaves && chave > no->C[pos])
    {
        pos++;
    }

    return pos;
}

// Insere chave e referencia mantendo as chaves ordenadas no nó com espaco
void inserirChaveOrdenadaNo(NoArvoreB *no, int chave, int referencia, int filhoDireita)
{
    int pos = no->nroChaves;

    while (pos > 0 && chave < no->C[pos - 1])
    {
        no->C[pos] = no->C[pos - 1];
        no->PR[pos] = no->PR[pos - 1];
        no->P[pos + 1] = no->P[pos];
        pos--;
    }

    no->C[pos] = chave;
    no->PR[pos] = referencia;
    no->P[pos + 1] = filhoDireita;
    no->nroChaves++;

    limparNoAtivo(no);
}

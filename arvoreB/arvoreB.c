#include "arvoreB.h"
#include "../cabecalhoArvoreB/cabecalhoArvoreB.h"
#include "../noArvoreB/noArvoreB.h"

typedef struct
{
    int chave;
    int referencia;
    int filhoDireita;
} Promocao;

// Inicializa o arquivo de indice com cabecalho inconsistente e arvore vazia
int criarArquivoIndiceArvoreB(FILE *arquivoIndice)
{
    CabecalhoArvoreB cabecalho = criarCabecalhoArvoreBVazio();
    return escreverCabecalhoArvoreB(arquivoIndice, &cabecalho);
}

// Aloca um RRN novo ou reaproveita um no removido da pilha
static int alocarNoArvoreB(FILE *arquivoIndice, CabecalhoArvoreB *cabecalho, int *rrn)
{
    if (cabecalho->topo != -1)
    {
        NoArvoreB removido;
        *rrn = cabecalho->topo;

        if (!lerNoArvoreB(arquivoIndice, *rrn, &removido))
            return 0;
        cabecalho->topo = removido.proximo;
    }
    else
    {
        *rrn = cabecalho->proxRRN;
        cabecalho->proxRRN++;
    }

    cabecalho->nroNos++;
    return escreverCabecalhoArvoreB(arquivoIndice, cabecalho);
}

// Busca sequencial dentro dos nos da arvore-B
static int buscarArvoreBRec(FILE *arquivoIndice, int rrn, int chave, int *referencia)
{
    if (rrn == -1)
        return 0;

    NoArvoreB no;
    if (!lerNoArvoreB(arquivoIndice, rrn, &no))
        return 0;

    int pos = buscarPosicaoNo(&no, chave);
    if (pos < no.nroChaves && no.C[pos] == chave)
    {
        *referencia = no.PR[pos];
        return 1;
    }

    return buscarArvoreBRec(arquivoIndice, no.P[pos], chave, referencia);
}

int buscarArvoreB(FILE *arquivoIndice, int chave, int *referencia)
{
    CabecalhoArvoreB cabecalho;
    if (!lerCabecalhoArvoreB(arquivoIndice, &cabecalho))
        return 0;
    if (cabecalho.status == '0')
        return 0;

    return buscarArvoreBRec(arquivoIndice, cabecalho.noRaiz, chave, referencia);
}

// Copia chaves, referencias e ponteiros de um no para vetores temporarios
static void copiarNoParaTemporarios(NoArvoreB *no, int chaves[], int referencias[], int ponteiros[])
{
    for (int i = 0; i < MAX_CHAVES_ARVORE_B; i++)
    {
        chaves[i] = no->C[i];
        referencias[i] = no->PR[i];
    }

    for (int i = 0; i < ORDEM_ARVORE_B; i++)
    {
        ponteiros[i] = no->P[i];
    }
}

// Insere uma promocao nos vetores temporarios de um no cheio
static void inserirEmTemporarios(NoArvoreB *no, Promocao nova, int chaves[], int referencias[], int ponteiros[])
{
    int pos = no->nroChaves;

    while (pos > 0 && nova.chave < chaves[pos - 1])
    {
        chaves[pos] = chaves[pos - 1];
        referencias[pos] = referencias[pos - 1];
        ponteiros[pos + 1] = ponteiros[pos];
        pos--;
    }

    chaves[pos] = nova.chave;
    referencias[pos] = nova.referencia;
    ponteiros[pos + 1] = nova.filhoDireita;
}

// Divide um no cheio e retorna a chave que deve subir para o pai
static int splitNoArvoreB(FILE *arquivoIndice, CabecalhoArvoreB *cabecalho, int rrn,
                          NoArvoreB *no, Promocao nova, Promocao *promovida)
{
    int chaves[4];
    int referencias[4];
    int ponteiros[5];
    int rrnDireita;

    for (int i = 0; i < 4; i++)
    {
        chaves[i] = -1;
        referencias[i] = -1;
    }

    for (int i = 0; i < 5; i++)
    {
        ponteiros[i] = -1;
    }

    copiarNoParaTemporarios(no, chaves, referencias, ponteiros);
    inserirEmTemporarios(no, nova, chaves, referencias, ponteiros);

    if (!alocarNoArvoreB(arquivoIndice, cabecalho, &rrnDireita))
        return 0;

    NoArvoreB direita = criarNoArvoreBVazio(no->tipoNo == 0 ? 1 : no->tipoNo);
    no->nroChaves = 2;
    no->C[0] = chaves[0];
    no->PR[0] = referencias[0];
    no->C[1] = chaves[1];
    no->PR[1] = referencias[1];
    no->C[2] = -1;
    no->PR[2] = -1;

    for (int i = 0; i < 4; i++)
    {
        no->P[i] = -1;
    }

    if (no->tipoNo != -1)
    {
        no->P[0] = ponteiros[0];
        no->P[1] = ponteiros[1];
        no->P[2] = ponteiros[2];
    }

    direita.nroChaves = 1;
    direita.C[0] = chaves[3];
    direita.PR[0] = referencias[3];

    if (direita.tipoNo != -1)
    {
        direita.P[0] = ponteiros[3];
        direita.P[1] = ponteiros[4];
    }

    promovida->chave = chaves[2];
    promovida->referencia = referencias[2];
    promovida->filhoDireita = rrnDireita;

    if (!escreverNoArvoreB(arquivoIndice, rrn, no))
        return 0;
    if (!escreverNoArvoreB(arquivoIndice, rrnDireita, &direita))
        return 0;

    return 1;
}

// Insere recursivamente; retorna 1 quando ha promocao para o nivel acima
static int inserirArvoreBRec(FILE *arquivoIndice, CabecalhoArvoreB *cabecalho, int rrn,
                             int chave, int referencia, Promocao *promovida, int *houvePromocao)
{
    NoArvoreB no;
    Promocao novaPromocao;
    int filhoPromoveu = 0;

    if (!lerNoArvoreB(arquivoIndice, rrn, &no))
        return 0;

    if (no.tipoNo == -1)
    {
        novaPromocao.chave = chave;
        novaPromocao.referencia = referencia;
        novaPromocao.filhoDireita = -1;

        if (no.nroChaves < MAX_CHAVES_ARVORE_B)
        {
            inserirChaveOrdenadaNo(&no, chave, referencia, -1);
            if (!escreverNoArvoreB(arquivoIndice, rrn, &no))
                return 0;
            *houvePromocao = 0;
            return 1;
        }

        if (!splitNoArvoreB(arquivoIndice, cabecalho, rrn, &no, novaPromocao, promovida))
            return 0;
        *houvePromocao = 1;
        return 1;
    }

    int pos = no.nroChaves;
    while (pos > 0 && chave < no.C[pos - 1])
    {
        pos--;
    }

    if (!inserirArvoreBRec(arquivoIndice, cabecalho, no.P[pos], chave, referencia,
                           &novaPromocao, &filhoPromoveu))
    {
        return 0;
    }

    if (!filhoPromoveu)
    {
        *houvePromocao = 0;
        return 1;
    }

    if (no.nroChaves < MAX_CHAVES_ARVORE_B)
    {
        inserirChaveOrdenadaNo(&no, novaPromocao.chave, novaPromocao.referencia, novaPromocao.filhoDireita);
        if (!escreverNoArvoreB(arquivoIndice, rrn, &no))
            return 0;
        *houvePromocao = 0;
        return 1;
    }

    if (!splitNoArvoreB(arquivoIndice, cabecalho, rrn, &no, novaPromocao, promovida))
        return 0;
    *houvePromocao = 1;
    return 1;
}

// Atualiza o tipo da raiz antiga quando ela deixa de ser raiz
static int transformarRaizAntigaEmFilho(FILE *arquivoIndice, int rrnRaizAntiga)
{
    NoArvoreB raizAntiga;
    if (!lerNoArvoreB(arquivoIndice, rrnRaizAntiga, &raizAntiga))
        return 0;

    if (raizAntiga.tipoNo == 0)
    {
        raizAntiga.tipoNo = 1;
        if (!escreverNoArvoreB(arquivoIndice, rrnRaizAntiga, &raizAntiga))
            return 0;
    }

    return 1;
}

// Insere uma chave na arvore-B, atualizando nos e cabecalho em disco
int inserirArvoreB(FILE *arquivoIndice, int chave, int referencia)
{
    CabecalhoArvoreB cabecalho;
    Promocao promovida;
    int houvePromocao = 0;

    if (!lerCabecalhoArvoreB(arquivoIndice, &cabecalho))
        return 0;

    if (cabecalho.noRaiz == -1)
    {
        int rrnRaiz;
        if (!alocarNoArvoreB(arquivoIndice, &cabecalho, &rrnRaiz))
            return 0;

        NoArvoreB raiz = criarNoArvoreBVazio(-1);
        raiz.nroChaves = 1;
        raiz.C[0] = chave;
        raiz.PR[0] = referencia;

        cabecalho.noRaiz = rrnRaiz;
        if (!escreverNoArvoreB(arquivoIndice, rrnRaiz, &raiz))
            return 0;
        return escreverCabecalhoArvoreB(arquivoIndice, &cabecalho);
    }

    int rrnRaizAntiga = cabecalho.noRaiz;
    if (!inserirArvoreBRec(arquivoIndice, &cabecalho, cabecalho.noRaiz, chave, referencia,
                           &promovida, &houvePromocao))
    {
        return 0;
    }

    if (houvePromocao)
    {
        int rrnNovaRaiz;
        if (!alocarNoArvoreB(arquivoIndice, &cabecalho, &rrnNovaRaiz))
            return 0;

        if (!transformarRaizAntigaEmFilho(arquivoIndice, rrnRaizAntiga))
            return 0;

        NoArvoreB novaRaiz = criarNoArvoreBVazio(0);
        novaRaiz.nroChaves = 1;
        novaRaiz.C[0] = promovida.chave;
        novaRaiz.PR[0] = promovida.referencia;
        novaRaiz.P[0] = rrnRaizAntiga;
        novaRaiz.P[1] = promovida.filhoDireita;

        cabecalho.noRaiz = rrnNovaRaiz;
        if (!escreverNoArvoreB(arquivoIndice, rrnNovaRaiz, &novaRaiz))
            return 0;
    }

    return escreverCabecalhoArvoreB(arquivoIndice, &cabecalho);
}

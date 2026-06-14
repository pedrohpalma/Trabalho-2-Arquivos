#include "noArvoreB.h"
#include "../cabecalhoArvoreB/cabecalhoArvoreB.h"

// Limpa os campos de chaves, referencias e filhos de um nó ativo para garantir que não haja lixo de memória
void limparNoAtivo(NoArvoreB *no)
{
    // Preenche os campos de chaves e referencias com -1 para indicar que estão vazios
    for (int i = no->nroChaves; i < MAX_CHAVES_ARVORE_B; i++)
    {
        no->C[i] = -1;
        no->PR[i] = -1;
    }

    // Preenche os campos de filhos com -1 para indicar que estão vazios
    for (int i = no->nroChaves + 1; i < ORDEM_ARVORE_B; i++)
    {
        no->P[i] = -1;
    }

    // Se o nó for folha, preenche todos os campos de filhos com -1
    if (no->tipoNo == -1)
    {
        for (int i = 0; i < ORDEM_ARVORE_B; i++)
        {
            no->P[i] = -1;
        }
    }
}

// Cria um nó da árvore-B vazio, inicializando os campos com valores padrão (nulos ou -1) e definindo o tipo do nó
NoArvoreB criarNoArvoreBVazio(int tipoNo)
{
    // Valores padrão da Árvore-B
    NoArvoreB no;
    no.removido = '0';
    no.proximo = -1; 
    no.tipoNo = tipoNo;
    no.nroChaves = 0;

    // Inicializa os campos de chaves, referencias e filhos com -1 para indicar que estão vazios
    for (int i = 0; i < MAX_CHAVES_ARVORE_B; i++)
    {
        no.C[i] = -1;
        no.PR[i] = -1;
    }

    // Preenche os campos de filhos com -1 para indicar que estão vazios
    for (int i = 0; i < ORDEM_ARVORE_B; i++)
    {
        no.P[i] = -1;
    }

    return no;
}


// Função que calcula o offset de um nó da árvore-B no arquivo de índice com base no RRN
long calcularOffsetNoArvoreB(int rrn)
{
    return TAM_CABECALHO_ARVORE_B + (rrn * TAM_NO_ARVORE_B);
}

// Lê um nó da árvore-B campo a campo, garantindo 53 bytes
int lerNoArvoreB(FILE *arquivoIndice, int rrn, NoArvoreB *no)
{
    // Move o ponteiro do arquivo para a posição correta do nó com base no RRN
    fseek(arquivoIndice, calcularOffsetNoArvoreB(rrn), SEEK_SET);

    // Faz a leituras dos campos do nó, verificando se há falhas
    if (fread(&no->removido, 1, 1, arquivoIndice) != 1)
        return 0;
    if (fread(&no->proximo, 4, 1, arquivoIndice) != 1)
        return 0;
    if (fread(&no->tipoNo, 4, 1, arquivoIndice) != 1)
        return 0;
    if (fread(&no->nroChaves, 4, 1, arquivoIndice) != 1)
        return 0;


    // Lê os campos de chaves, referencias e filhos do nó
    for (int i = 0; i < MAX_CHAVES_ARVORE_B; i++)
    {
        if (fread(&no->C[i], 4, 1, arquivoIndice) != 1)
            return 0;
        if (fread(&no->PR[i], 4, 1, arquivoIndice) != 1)
            return 0;
    }

    // Lê os campos de filhos do nó
    for (int i = 0; i < ORDEM_ARVORE_B; i++)
    {
        if (fread(&no->P[i], 4, 1, arquivoIndice) != 1)
            return 0;
    }

    return 1;
}

// Escreve um nó da árvore-B campo a campo, garantindo 53 bytes
int escreverNoArvoreB(FILE *arquivoIndice, int rrn, NoArvoreB *no)
{
    //Limpa os campos de chaves, referencias e filhos do nó para garantir que não haja lixo de memória antes de escrever no arquivo.
    limparNoAtivo(no);
    fseek(arquivoIndice, calcularOffsetNoArvoreB(rrn), SEEK_SET);

    // Faz a escrita dos campos do nó, verificando se há falhas
    if (fwrite(&no->removido, 1, 1, arquivoIndice) != 1)
        return 0;
    if (fwrite(&no->proximo, 4, 1, arquivoIndice) != 1)
        return 0;
    if (fwrite(&no->tipoNo, 4, 1, arquivoIndice) != 1)
        return 0;
    if (fwrite(&no->nroChaves, 4, 1, arquivoIndice) != 1)
        return 0;

    // Escreve os campos de chaves, referencias e filhos do nó
    for (int i = 0; i < MAX_CHAVES_ARVORE_B; i++)
    {
        if (fwrite(&no->C[i], 4, 1, arquivoIndice) != 1)
            return 0;
        if (fwrite(&no->PR[i], 4, 1, arquivoIndice) != 1)
            return 0;
    }

    // Escreve os campos de filhos do nó
    for (int i = 0; i < ORDEM_ARVORE_B; i++)
    {
        if (fwrite(&no->P[i], 4, 1, arquivoIndice) != 1)
            return 0;
    }

    fflush(arquivoIndice);
    return 1;
}

// Busca a posição de forma ordenada de onde a chave deve ser inserida ou onde ela está presente em um nó da árvore-B e retorna essa posição.
int buscarPosicaoNo(NoArvoreB *no, int chave)
{
    int pos = 0;

    while (pos < no->nroChaves && chave > no->C[pos])
    {
        pos++;
    }

    return pos;
}

// Insere uma chave, referência e filho direito em um nó da árvore-B de forma ordenada, garantindo que as chaves permaneçam em ordem crescente.
void inserirChaveOrdenadaNo(NoArvoreB *no, int chave, int referencia, int filhoDireita)
{
    int pos = no->nroChaves;

    //Desloca elementos à direita para inserir a nova entrada mantendo a ordenação.
    while (pos > 0 && chave < no->C[pos - 1])
    {
        no->C[pos] = no->C[pos - 1];
        no->PR[pos] = no->PR[pos - 1];
        no->P[pos + 1] = no->P[pos];
        pos--;
    }

    // Insere a nova chave, referência e filho direito na posição correta.
    no->C[pos] = chave;
    no->PR[pos] = referencia;
    no->P[pos + 1] = filhoDireita;
    no->nroChaves++;

    limparNoAtivo(no);
}

#include "arvoreB.h"
#include "../cabecalhoArvoreB/cabecalhoArvoreB.h"
#include "../noArvoreB/noArvoreB.h"

typedef struct // Definicao do tipo que é usado para retornar dados sobre promoçoes realizadas recursivamente
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
int alocarNoArvoreB(FILE *arquivoIndice, CabecalhoArvoreB *cabecalho, int *rrn)
{
    if (cabecalho->topo != -1) // há nó removido logicamente
    {
        NoArvoreB removido;
        *rrn = cabecalho->topo;

        if (!lerNoArvoreB(arquivoIndice, *rrn, &removido)) // pega os dados do removido e muda topo da pilha
            return 0;
        cabecalho->topo = removido.proximo;
    }
    else // nao tem pilha de removidos logicamente
    {
        *rrn = cabecalho->proxRRN; // pega prox rrn livre e soma 1 ao total
        cabecalho->proxRRN++;
    }

    cabecalho->nroNos++;
    return escreverCabecalhoArvoreB(arquivoIndice, cabecalho); // reescreve cabecalho no arquivo com infos atualizadas
}

// Busca recursiva dentro dos nos da arvore-B
int buscarArvoreBRec(FILE *arquivoIndice, int rrn, int chave, int *referencia)
{
    if (rrn == -1) // arvore vazia
        return 0;

    NoArvoreB no;
    if (!lerNoArvoreB(arquivoIndice, rrn, &no)) // le nó em questão e salva localmente
        return 0;

    int pos = buscarPosicaoNo(&no, chave);        // busca se chave existe no nó/para qual nó descer
    if (pos < no.nroChaves && no.C[pos] == chave) // caso em que achou
    {
        *referencia = no.PR[pos];
        return 1;
    }

    return buscarArvoreBRec(arquivoIndice, no.P[pos], chave, referencia); // busca denovo descendo no filho correto
}

int buscarArvoreB(FILE *arquivoIndice, int chave, int *referencia)
{
    CabecalhoArvoreB cabecalho;
    if (!lerCabecalhoArvoreB(arquivoIndice, &cabecalho)) // le o cabecalho da arvore e começa busca no nó raiz
        return 0;

    return buscarArvoreBRec(arquivoIndice, cabecalho.noRaiz, chave, referencia);
}

// Copia chaves, referencias e ponteiros de um no para vetores temporarios(usado no split)
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

// Insere uma promocao nos vetores temporarios de um nó cheio(usado no split)
static void inserirEmTemporarios(NoArvoreB *no, Promocao nova, int chaves[], int referencias[], int ponteiros[])
{
    int pos = no->nroChaves;

    while (pos > 0 && nova.chave < chaves[pos - 1]) // encontra a promocao no local certo, atualizando os vetores todos
    {
        chaves[pos] = chaves[pos - 1];
        referencias[pos] = referencias[pos - 1];
        ponteiros[pos + 1] = ponteiros[pos];
        pos--;
    }

    chaves[pos] = nova.chave; // realiza a gravaçao
    referencias[pos] = nova.referencia;
    ponteiros[pos + 1] = nova.filhoDireita;
}

// Divide um no cheio passando os dois menores para o nó original, promovendo o terceiro e colocando o maior no novo nó a direita. Também devolve informaçoes sobre promocao
static int splitNoArvoreB(FILE *arquivoIndice, CabecalhoArvoreB *cabecalho, int rrn,
                          NoArvoreB *no, Promocao nova, Promocao *promovida)
{
    int chaves[4]; // quatro chaves temporariamente, duas menores ficam na esquerda, terceira é promovida, quarta vai para novo nó na direita
    int referencias[4];
    int ponteiros[5]; // temporariamente cinco ponteiros
    int rrnDireita;

    for (int i = 0; i < 4; i++) // inicializa os vetores temporarios vazios
    {
        chaves[i] = -1;
        referencias[i] = -1;
    }

    for (int i = 0; i < 5; i++)
    {
        ponteiros[i] = -1;
    }

    copiarNoParaTemporarios(no, chaves, referencias, ponteiros);    // utiza auxiliar para copiar coisas para nó temporario
    inserirEmTemporarios(no, nova, chaves, referencias, ponteiros); // insere nova chave no nó temporario

    if (!alocarNoArvoreB(arquivoIndice, cabecalho, &rrnDireita)) // aloca novo nó na direita
        return 0;

    NoArvoreB direita = criarNoArvoreBVazio(no->tipoNo == 0 ? 1 : no->tipoNo);
    no->nroChaves = 2; // organiza o nó original mantendo apenas as duas menores chaves
    no->C[0] = chaves[0];
    no->PR[0] = referencias[0];
    no->C[1] = chaves[1];
    no->PR[1] = referencias[1];
    no->C[2] = -1;
    no->PR[2] = -1;

    for (int i = 0; i < 4; i++) // zera inicialmente os ponteiros
    {
        no->P[i] = -1;
    }

    if (no->tipoNo != -1) // aloca os ponteiros corretos caso o nó nao seja folha(como tem duas chaves tem tres filhos)
    {
        no->P[0] = ponteiros[0];
        no->P[1] = ponteiros[1];
        no->P[2] = ponteiros[2];
    }

    direita.nroChaves = 1; // atualiza as infos do novo nó criado na direita, que recebe a maior chave
    direita.C[0] = chaves[3];
    direita.PR[0] = referencias[3];

    if (direita.tipoNo != -1) // se nao é folha, atualiza os ponteiros
    {
        direita.P[0] = ponteiros[3];
        direita.P[1] = ponteiros[4];
    }

    promovida->chave = chaves[2]; // recebe os dados da chave que será promovida
    promovida->referencia = referencias[2];
    promovida->filhoDireita = rrnDireita;

    if (!escreverNoArvoreB(arquivoIndice, rrn, no)) // escreve no original e novo no a direita em disco
        return 0;
    if (!escreverNoArvoreB(arquivoIndice, rrnDireita, &direita))
        return 0;

    return 1;
}

// Insere recursivamente; retorna 1 em *houvePromocao quando ha promocao para o nivel acima
static int inserirArvoreBRec(FILE *arquivoIndice, CabecalhoArvoreB *cabecalho, int rrn,
                             int chave, int referencia, Promocao *promovida, int *houvePromocao)
{
    NoArvoreB no;
    Promocao novaPromocao;
    int filhoPromoveu = 0;

    if (rrn == -1) // caso de árvore vazia ou ponteiro novo, força uma promoção para adicionar novo nó
    {
        promovida->chave = chave;
        promovida->referencia = referencia;
        promovida->filhoDireita = -1;
        *houvePromocao = 1;
        return 1;
    }

    if (!lerNoArvoreB(arquivoIndice, rrn, &no)) // le o nó e encontra em qual posiçao correta dele
        return 0;

    int pos = buscarPosicaoNo(&no, chave);

    if (no.tipoNo == -1) // caso em que nó é folha
    {
        novaPromocao.chave = chave;
        novaPromocao.referencia = referencia; // cria promoção com infos novas caso haja necessidade de split
        novaPromocao.filhoDireita = -1;

        if (no.nroChaves < MAX_CHAVES_ARVORE_B) // caso chave caiba no nó e nao precise de split
        {
            inserirChaveOrdenadaNo(&no, chave, referencia, -1); // insire normalmente
            if (!escreverNoArvoreB(arquivoIndice, rrn, &no))
                return 0;
            *houvePromocao = 0;
            return 1;
        }

        if (!splitNoArvoreB(arquivoIndice, cabecalho, rrn, &no, novaPromocao, promovida)) // faz split caso nao caiba no nó
            return 0;
        *houvePromocao = 1;
        return 1;
    }

    if (!inserirArvoreBRec(arquivoIndice, cabecalho, no.P[pos], chave, referencia,
                           &novaPromocao, &filhoPromoveu)) // caso em que nao é folha, desce para o filho correto
    {
        return 0;
    }

    if (!filhoPromoveu) // se o filho nao promoveu nada
    {
        *houvePromocao = 0;
        return 1;
    }

    // daqui pra baixo, situação em que houve promoçao vinda do filho
    if (no.nroChaves < MAX_CHAVES_ARVORE_B) // insere normalmente caso haja espaço
    {
        inserirChaveOrdenadaNo(&no, novaPromocao.chave, novaPromocao.referencia, novaPromocao.filhoDireita);
        if (!escreverNoArvoreB(arquivoIndice, rrn, &no))
            return 0;
        *houvePromocao = 0;
        return 1;
    }

    if (!splitNoArvoreB(arquivoIndice, cabecalho, rrn, &no, novaPromocao, promovida)) // splita caso nao tenha espaço
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

    if (!lerCabecalhoArvoreB(arquivoIndice, &cabecalho)) // começa lendo cabeçalho para pegar infos
        return 0;

    if (cabecalho.noRaiz == -1) // caso arvore vazia
    {
        int rrnRaiz;
        if (!alocarNoArvoreB(arquivoIndice, &cabecalho, &rrnRaiz)) // aloca rrn
            return 0;

        NoArvoreB raiz = criarNoArvoreBVazio(-1);
        raiz.nroChaves = 1;
        raiz.C[0] = chave; // passa infos da raiz
        raiz.PR[0] = referencia;

        cabecalho.noRaiz = rrnRaiz;                            // atualiza cabecalho
        if (!escreverNoArvoreB(arquivoIndice, rrnRaiz, &raiz)) // escreve em disco
            return 0;
        return escreverCabecalhoArvoreB(arquivoIndice, &cabecalho);
    }

    int rrnRaizAntiga = cabecalho.noRaiz; // guarda rrn da raiz antiga
    if (!inserirArvoreBRec(arquivoIndice, &cabecalho, cabecalho.noRaiz, chave, referencia,
                           &promovida, &houvePromocao)) // chama inserçao recursiva começando da raiz
    {
        return 0;
    }

    if (houvePromocao) // caso em que ocorre split na raiz, logo raiz muda
    {
        int rrnNovaRaiz;
        if (!alocarNoArvoreB(arquivoIndice, &cabecalho, &rrnNovaRaiz)) // aloca rrn da nova raiz
            return 0;

        if (!transformarRaizAntigaEmFilho(arquivoIndice, rrnRaizAntiga)) // chama funçao pra transformar antiga em filha
            return 0;

        NoArvoreB novaRaiz = criarNoArvoreBVazio(0); // cria nova raiz preenchendo com os dados da chave promovida
        novaRaiz.nroChaves = 1;
        novaRaiz.C[0] = promovida.chave;
        novaRaiz.PR[0] = promovida.referencia;
        novaRaiz.P[0] = rrnRaizAntiga;
        novaRaiz.P[1] = promovida.filhoDireita;

        cabecalho.noRaiz = rrnNovaRaiz;
        if (!escreverNoArvoreB(arquivoIndice, rrnNovaRaiz, &novaRaiz))
            return 0;
    }

    return escreverCabecalhoArvoreB(arquivoIndice, &cabecalho); // escreve cabeçalho atualizado em disco
}

static int noEhFolha(NoArvoreB *no) // checa se nó é folha
{
    return no->tipoNo == -1;
}

static int escreverNoRemovidoArvoreB(FILE *arquivoIndice, int rrn, NoArvoreB *no)
{
    fseek(arquivoIndice, calcularOffsetNoArvoreB(rrn), SEEK_SET);
    if (fwrite(&no->removido, 1, 1, arquivoIndice) != 1)
        return 0;
    if (fwrite(&no->proximo, 4, 1, arquivoIndice) != 1)
        return 0;
    fflush(arquivoIndice);
    return 1;
}

static int liberarNoArvoreB(FILE *arquivoIndice, CabecalhoArvoreB *cabecalho, int rrn)
{
    NoArvoreB no;
    if (!lerNoArvoreB(arquivoIndice, rrn, &no))
        return 0;

    no.removido = '1';
    no.proximo = cabecalho->topo;
    if (!escreverNoRemovidoArvoreB(arquivoIndice, rrn, &no))
        return 0;

    cabecalho->topo = rrn;
    cabecalho->nroNos--;
    return escreverCabecalhoArvoreB(arquivoIndice, cabecalho);
}

static void removerChaveDoNo(NoArvoreB *no, int pos)
{
    for (int i = pos; i < no->nroChaves - 1; i++)
    {
        no->C[i] = no->C[i + 1];
        no->PR[i] = no->PR[i + 1];
    }

    no->nroChaves--;
    limparNoAtivo(no);
}

static void removerSeparadorDoPai(NoArvoreB *pai, int posSeparador)
{
    int antigoNroChaves = pai->nroChaves;

    for (int i = posSeparador; i < antigoNroChaves - 1; i++)
    {
        pai->C[i] = pai->C[i + 1];
        pai->PR[i] = pai->PR[i + 1];
    }

    for (int i = posSeparador + 1; i < antigoNroChaves; i++)
    {
        pai->P[i] = pai->P[i + 1];
    }

    pai->nroChaves--;
    limparNoAtivo(pai);
}

static int encontrarSucessorEmFolha(FILE *arquivoIndice, int rrnAtual, int *chave, int *referencia)
{
    NoArvoreB no;

    while (rrnAtual != -1)
    {
        if (!lerNoArvoreB(arquivoIndice, rrnAtual, &no))
            return 0;

        if (noEhFolha(&no))
        {
            if (no.nroChaves == 0)
                return 0;
            *chave = no.C[0];
            *referencia = no.PR[0];
            return 1;
        }

        rrnAtual = no.P[0];
    }

    return 0;
}

static void juntarNosComSeparador(NoArvoreB *esquerda, NoArvoreB *direita, NoArvoreB *pai,
                                  int posSeparador, int chaves[], int referencias[], int ponteiros[])
{
    int qtd = 0;

    for (int i = 0; i < 5; i++)
        ponteiros[i] = -1;

    if (!noEhFolha(esquerda))
    {
        for (int i = 0; i <= esquerda->nroChaves; i++)
            ponteiros[i] = esquerda->P[i];
    }

    for (int i = 0; i < esquerda->nroChaves; i++)
    {
        chaves[qtd] = esquerda->C[i];
        referencias[qtd] = esquerda->PR[i];
        qtd++;
    }

    chaves[qtd] = pai->C[posSeparador];
    referencias[qtd] = pai->PR[posSeparador];
    qtd++;

    if (!noEhFolha(direita))
    {
        int inicio = esquerda->nroChaves + 1;
        for (int i = 0; i <= direita->nroChaves; i++)
            ponteiros[inicio + i] = direita->P[i];
    }

    for (int i = 0; i < direita->nroChaves; i++)
    {
        chaves[qtd] = direita->C[i];
        referencias[qtd] = direita->PR[i];
        qtd++;
    }
}

static void distribuirEntreNos(NoArvoreB *esquerda, NoArvoreB *direita, NoArvoreB *pai,
                               int posSeparador, int chaves[], int referencias[], int ponteiros[], int totalChaves)
{
    int qtdEsquerda = totalChaves / 2;
    int posPromovida = qtdEsquerda;
    int qtdDireita = totalChaves - qtdEsquerda - 1;

    esquerda->nroChaves = qtdEsquerda;
    direita->nroChaves = qtdDireita;

    for (int i = 0; i < MAX_CHAVES_ARVORE_B; i++)
    {
        esquerda->C[i] = -1;
        esquerda->PR[i] = -1;
        direita->C[i] = -1;
        direita->PR[i] = -1;
    }

    for (int i = 0; i < ORDEM_ARVORE_B; i++)
    {
        esquerda->P[i] = -1;
        direita->P[i] = -1;
    }

    for (int i = 0; i < qtdEsquerda; i++)
    {
        esquerda->C[i] = chaves[i];
        esquerda->PR[i] = referencias[i];
    }

    pai->C[posSeparador] = chaves[posPromovida];
    pai->PR[posSeparador] = referencias[posPromovida];

    for (int i = 0; i < qtdDireita; i++)
    {
        direita->C[i] = chaves[posPromovida + 1 + i];
        direita->PR[i] = referencias[posPromovida + 1 + i];
    }

    if (!noEhFolha(esquerda))
    {
        for (int i = 0; i <= qtdEsquerda; i++)
            esquerda->P[i] = ponteiros[i];
        for (int i = 0; i <= qtdDireita; i++)
            direita->P[i] = ponteiros[posPromovida + 1 + i];
    }

    limparNoAtivo(esquerda);
    limparNoAtivo(direita);
    limparNoAtivo(pai);
}

static int redistribuirNos(FILE *arquivoIndice, int rrnPai, int posSeparador, int rrnEsquerda, int rrnDireita)
{
    NoArvoreB pai, esquerda, direita;
    int chaves[4], referencias[4], ponteiros[5];

    if (!lerNoArvoreB(arquivoIndice, rrnPai, &pai))
        return 0;
    if (!lerNoArvoreB(arquivoIndice, rrnEsquerda, &esquerda))
        return 0;
    if (!lerNoArvoreB(arquivoIndice, rrnDireita, &direita))
        return 0;

    juntarNosComSeparador(&esquerda, &direita, &pai, posSeparador, chaves, referencias, ponteiros);
    distribuirEntreNos(&esquerda, &direita, &pai, posSeparador, chaves, referencias, ponteiros,
                       esquerda.nroChaves + direita.nroChaves + 1);

    if (!escreverNoArvoreB(arquivoIndice, rrnEsquerda, &esquerda))
        return 0;
    if (!escreverNoArvoreB(arquivoIndice, rrnDireita, &direita))
        return 0;
    if (!escreverNoArvoreB(arquivoIndice, rrnPai, &pai))
        return 0;

    return 1;
}

static int concatenarNos(FILE *arquivoIndice, CabecalhoArvoreB *cabecalho, int rrnPai,
                         int posSeparador, int rrnEsquerda, int rrnDireita)
{
    NoArvoreB pai, esquerda, direita;
    int pos = 0;

    if (!lerNoArvoreB(arquivoIndice, rrnPai, &pai))
        return 0;
    if (!lerNoArvoreB(arquivoIndice, rrnEsquerda, &esquerda))
        return 0;
    if (!lerNoArvoreB(arquivoIndice, rrnDireita, &direita))
        return 0;

    pos = esquerda.nroChaves;
    esquerda.C[pos] = pai.C[posSeparador];
    esquerda.PR[pos] = pai.PR[posSeparador];
    pos++;

    for (int i = 0; i < direita.nroChaves; i++)
    {
        esquerda.C[pos] = direita.C[i];
        esquerda.PR[pos] = direita.PR[i];
        pos++;
    }

    if (!noEhFolha(&esquerda))
    {
        int inicio = esquerda.nroChaves + 1;
        for (int i = 0; i <= direita.nroChaves; i++)
            esquerda.P[inicio + i] = direita.P[i];
    }

    esquerda.nroChaves = pos;
    limparNoAtivo(&esquerda);
    removerSeparadorDoPai(&pai, posSeparador);

    if (!escreverNoArvoreB(arquivoIndice, rrnEsquerda, &esquerda))
        return 0;
    if (!escreverNoArvoreB(arquivoIndice, rrnPai, &pai))
        return 0;
    if (!liberarNoArvoreB(arquivoIndice, cabecalho, rrnDireita))
        return 0;

    return 1;
}

static int tratarUnderflow(FILE *arquivoIndice, CabecalhoArvoreB *cabecalho, int rrnPai, int indiceFilho)
{
    NoArvoreB pai, irmao;

    if (!lerNoArvoreB(arquivoIndice, rrnPai, &pai))
        return 0;

    if (indiceFilho < pai.nroChaves && pai.P[indiceFilho + 1] != -1)
    {
        if (!lerNoArvoreB(arquivoIndice, pai.P[indiceFilho + 1], &irmao))
            return 0;
        if (irmao.nroChaves > 1)
            return redistribuirNos(arquivoIndice, rrnPai, indiceFilho, pai.P[indiceFilho], pai.P[indiceFilho + 1]);
    }

    if (indiceFilho > 0 && pai.P[indiceFilho - 1] != -1)
    {
        if (!lerNoArvoreB(arquivoIndice, pai.P[indiceFilho - 1], &irmao))
            return 0;
        if (irmao.nroChaves > 1)
            return redistribuirNos(arquivoIndice, rrnPai, indiceFilho - 1, pai.P[indiceFilho - 1], pai.P[indiceFilho]);
    }

    if (indiceFilho > 0 && pai.P[indiceFilho - 1] != -1)
        return concatenarNos(arquivoIndice, cabecalho, rrnPai, indiceFilho - 1, pai.P[indiceFilho - 1], pai.P[indiceFilho]);

    if (indiceFilho < pai.nroChaves && pai.P[indiceFilho + 1] != -1)
        return concatenarNos(arquivoIndice, cabecalho, rrnPai, indiceFilho, pai.P[indiceFilho], pai.P[indiceFilho + 1]);

    return 1;
}

static int removerArvoreBRec(FILE *arquivoIndice, CabecalhoArvoreB *cabecalho, int rrn,
                             int chave, int ehRaiz, int *encontrou, int *underflow)
{
    NoArvoreB no;

    *encontrou = 0;
    *underflow = 0;

    if (rrn == -1)
        return 1;

    if (!lerNoArvoreB(arquivoIndice, rrn, &no))
        return 0;

    int pos = buscarPosicaoNo(&no, chave);

    if (pos < no.nroChaves && no.C[pos] == chave)
    {
        *encontrou = 1;

        if (noEhFolha(&no))
        {
            removerChaveDoNo(&no, pos);
            if (!escreverNoArvoreB(arquivoIndice, rrn, &no))
                return 0;

            *underflow = (!ehRaiz && no.nroChaves < 1);
            return 1;
        }

        int chaveSucessora, referenciaSucessora;
        int encontrouFilho, underflowFilho;

        if (!encontrarSucessorEmFolha(arquivoIndice, no.P[pos + 1], &chaveSucessora, &referenciaSucessora))
            return 0;

        no.C[pos] = chaveSucessora;
        no.PR[pos] = referenciaSucessora;
        if (!escreverNoArvoreB(arquivoIndice, rrn, &no))
            return 0;

        if (!removerArvoreBRec(arquivoIndice, cabecalho, no.P[pos + 1], chaveSucessora, 0,
                               &encontrouFilho, &underflowFilho))
            return 0;

        if (underflowFilho)
        {
            if (!tratarUnderflow(arquivoIndice, cabecalho, rrn, pos + 1))
                return 0;
        }

        if (!lerNoArvoreB(arquivoIndice, rrn, &no))
            return 0;
        *underflow = (!ehRaiz && no.nroChaves < 1);
        return 1;
    }

    if (noEhFolha(&no))
        return 1;

    if (no.P[pos] == -1)
        return 1;

    int encontrouFilho, underflowFilho;
    if (!removerArvoreBRec(arquivoIndice, cabecalho, no.P[pos], chave, 0, &encontrouFilho, &underflowFilho))
        return 0;

    *encontrou = encontrouFilho;

    if (underflowFilho)
    {
        if (!tratarUnderflow(arquivoIndice, cabecalho, rrn, pos))
            return 0;
    }

    if (!lerNoArvoreB(arquivoIndice, rrn, &no))
        return 0;
    *underflow = (!ehRaiz && no.nroChaves < 1);
    return 1;
}

static int ajustarTipoRaiz(FILE *arquivoIndice, int rrnRaiz)
{
    NoArvoreB raiz;
    if (!lerNoArvoreB(arquivoIndice, rrnRaiz, &raiz))
        return 0;

    if (raiz.tipoNo != -1)
        raiz.tipoNo = 0;

    return escreverNoArvoreB(arquivoIndice, rrnRaiz, &raiz);
}

static int ajustarRaizAposRemocao(FILE *arquivoIndice, CabecalhoArvoreB *cabecalho)
{
    if (cabecalho->noRaiz == -1)
        return 1;

    NoArvoreB raiz;
    int rrnRaizAntiga = cabecalho->noRaiz;

    if (!lerNoArvoreB(arquivoIndice, cabecalho->noRaiz, &raiz))
        return 0;

    if (raiz.nroChaves > 0)
        return ajustarTipoRaiz(arquivoIndice, cabecalho->noRaiz);

    if (noEhFolha(&raiz))
    {
        cabecalho->noRaiz = -1;
        if (!liberarNoArvoreB(arquivoIndice, cabecalho, rrnRaizAntiga))
            return 0;
        return escreverCabecalhoArvoreB(arquivoIndice, cabecalho);
    }

    int rrnNovaRaiz = raiz.P[0];
    cabecalho->noRaiz = rrnNovaRaiz;

    if (!liberarNoArvoreB(arquivoIndice, cabecalho, rrnRaizAntiga))
        return 0;
    if (rrnNovaRaiz != -1 && !ajustarTipoRaiz(arquivoIndice, rrnNovaRaiz))
        return 0;

    return escreverCabecalhoArvoreB(arquivoIndice, cabecalho);
}

int removerArvoreB(FILE *arquivoIndice, int chave)
{
    CabecalhoArvoreB cabecalho;
    int encontrou, underflow;

    if (!lerCabecalhoArvoreB(arquivoIndice, &cabecalho))
        return 0;

    if (cabecalho.noRaiz == -1)
        return 1;

    if (!removerArvoreBRec(arquivoIndice, &cabecalho, cabecalho.noRaiz, chave, 1, &encontrou, &underflow))
        return 0;

    if (!ajustarRaizAposRemocao(arquivoIndice, &cabecalho))
        return 0;

    return escreverCabecalhoArvoreB(arquivoIndice, &cabecalho);
}

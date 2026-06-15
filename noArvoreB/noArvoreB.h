#ifndef NO_ARVORE_B_H
#define NO_ARVORE_B_H

#include <stdio.h>

#define TAM_NO_ARVORE_B 53
#define ORDEM_ARVORE_B 4
#define MAX_CHAVES_ARVORE_B 3


// Struct de um no da Arvore-B
typedef struct
{
    char removido;
    int proximo;
    int tipoNo;
    int nroChaves;
    int C[3];
    int PR[3];
    int P[4];
} NoArvoreB;

// Limpa os campos de chaves, referencias e filhos de um nó ativo para garantir que não haja lixo de memória
void limparNoAtivo(NoArvoreB *no);

// Inicializa e retorna um novo no na memoria.
NoArvoreB criarNoArvoreBVazio(int tipoNo);

// Converte o RRN logico de um no para o byte offset no arquivo, pulando o cabecalho.
long calcularOffsetNoArvoreB(int rrn);

// Le os 53 bytes de um no do arquivo para a memoria baseando-se no seu RRN.
int lerNoArvoreB(FILE *arquivoIndice, int rrn, NoArvoreB *no);

// Limpa o lixo de memoria e grava os dados do no no arquivo na posicao do RRN exata.
int escreverNoArvoreB(FILE *arquivoIndice, int rrn, NoArvoreB *no);

// Realiza busca linear no no para achar o indice da chave ou o indice do ponteiro pelo qual a busca deve descer.
int buscarPosicaoNo(NoArvoreB *no, int chave);

// Desloca elementos a direita e insere chave, referencia e ponteiro mantendo a ordem crescente. Assume que ha espaco.
void inserirChaveOrdenadaNo(NoArvoreB *no, int chave, int referencia, int filhoDireita);

// Preenche com -1 os espacos vazios (lixo de memoria) nos vetores C, PR e P do no para evitar gravar lixo no disco.
void inicializarCamposNaoUsadosComMenosUm(NoArvoreB *no);

#endif
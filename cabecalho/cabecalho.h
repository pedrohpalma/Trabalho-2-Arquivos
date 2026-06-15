#ifndef CABECALHO_H
#define CABECALHO_H
#include <stdio.h>


//Tamanho fixo do cabeçalho no arquivo binário, em bytes:
#define TAM_CABECALHO 17

typedef struct {
    char status;           // '0' inconsistente, '1' consistente
    int topo;              // RRN do topo da pilha de removidos (-1 se vazia)
    int proxRRN;           // Próximo RRN disponível para inserção
    int nroEstacoes;       // Quantidade de estações com nomes únicos
    int nroParesEstacao;   // Quantidade de pares únicos (codEstacao, codProxEstacao)
} Cabecalho;


// Inicializa o Cabecalho com os valores padrão
//(status='0', topo=-1, proxRRN=0, nroEstacoes=0, nroParesEstacao=0)
void initCabecalho(Cabecalho *c);

// Serializa e escreve a struct Cabecalho na posição atual do arquivo
void escreveCabecalho(FILE *bin, Cabecalho *c);

// Define status='1' (consistente) e reescreve o cabecalho no inicio do arquivo
void atualizaCabecalho(FILE *bin, Cabecalho *c);

// Lê o cabeçalho do início do arquivo binário para a struct 'c'.
int leCabecalho(FILE *bin, Cabecalho *c);

/*
 * Percorre todos os registros ativos do arquivo e recalcula do zero
 * os campos nroEstacoes e nroParesEstacao do Cabecalho.
 * Deve ser chamada após operações de inserção ou remoção que alteram
 * o conjunto de estações e pares presentes no arquivo.
 */

// Atualiza as contagens de estações e pares, evitando contagens duplicadas
void atualizaContagemEstacoes(FILE *bin, Cabecalho *c);

#endif

#ifndef CABECALHO_H
#define CABECALHO_H
#include <stdio.h>

/*
 * Tamanho fixo do cabeçalho no arquivo binário, em bytes:
 * 1 (status) + 4 (topo) + 4 (proxRRN) + 4 (nroEstacoes) + 4 (nroParesEstacao) = 17
 */
#define TAM_CABECALHO 17

/*
 * Metadados do arquivo binário, armazenados nos primeiros TAM_CABECALHO bytes.
 * 'status' controla consistência: '0' = arquivo em uso/inconsistente,
 * '1' = arquivo consistente e pronto para leitura.
 * 'topo' é o RRN do topo da pilha de registros removidos logicamente (-1 = pilha vazia).
 * 'proxRRN' é o próximo RRN disponível para inserção ao final do arquivo.
 * 'nroEstacoes' e 'nroParesEstacao' contam entidades únicas presentes.
 */
typedef struct {
    char status;           // '0' inconsistente, '1' consistente
    int topo;              // RRN do topo da pilha de removidos (-1 se vazia)
    int proxRRN;           // Próximo RRN disponível para inserção
    int nroEstacoes;       // Quantidade de estações com nomes únicos
    int nroParesEstacao;   // Quantidade de pares únicos (codEstacao, codProxEstacao)
} Cabecalho;

/*
 * Inicializa todos os campos do Cabecalho com os valores padrão:
 * status='0', topo=-1, proxRRN=0, nroEstacoes=0, nroParesEstacao=0.
 * Deve ser chamada antes de escrever um novo arquivo binário.
 */
void initCabecalho(Cabecalho *c);

/*
 * Serializa e escreve a struct Cabecalho na posição atual do arquivo 'bin'.
 * Deve ser chamada logo após o fseek para a posição 0 quando necessário.
 */
void escreveCabecalho(FILE *bin, Cabecalho *c);

/*
 * Define status='1' (consistente) e reescreve o cabeçalho no início do
 * arquivo. Deve ser chamada ao final de qualquer operação de escrita
 * para sinalizar que o arquivo está íntegro.
 */
void atualizaCabecalho(FILE *bin, Cabecalho *c);

/*
 * Lê o cabeçalho do início do arquivo binário para a struct 'c'.
 * Retorna 1 em caso de sucesso.
 * Retorna 0 se não conseguir ler o primeiro byte ou se status == '0'
 * (arquivo inconsistente, possivelmente corrompido).
 */
int leCabecalho(FILE *bin, Cabecalho *c);

/*
 * Percorre todos os registros ativos do arquivo e recalcula do zero
 * os campos nroEstacoes e nroParesEstacao do Cabecalho.
 * Deve ser chamada após operações de inserção ou remoção que alteram
 * o conjunto de estações e pares presentes no arquivo.
 */
void atualizaContagemEstacoes(FILE *bin, Cabecalho *c);

#endif

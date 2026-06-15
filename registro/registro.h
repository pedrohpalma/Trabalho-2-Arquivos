#ifndef REGISTRO_H
#define REGISTRO_H
#include <stdio.h>

#define TAM_REGISTRO 80

// Struct do registro de estação armazenado no arquivo binário
typedef struct{
    char removido;
    int proximo;
    int codEstacao;
    int codLinha;
    int codProxEstacao;
    int distProxEstacao;
    int codLinhaIntegra;
    int codEstIntegra;
    int tamNomeEstacao;
    char nomeEstacao[100];
    int tamNomeLinha;
    char nomeLinha[100];
} Registro;


// Struct dos campos usados para a busca
typedef struct {
    char nomeCampo[50]; 
    int valorInt; 
    char valorStr[100];
    int isNulo;
} CampoBusca;

// Lê uma linha do CSV e preenche a struct Registro
void lerRegistroCSV(char *linha, Registro *r);

// Serializa e escreve o Registro no arquivo binário
void escreveRegistroBin(FILE *bin, Registro *r);

// Lê exatamente TAM_REGISTRO bytes do arquivo binário e desserializa para a struct Registro.
int leRegistroBin(FILE *bin, Registro *r);

// Imprime os campos do Registro
void imprimeRegistro(Registro *r);

// Verifica se o Registro satisfaz um critério de busca
int atendeCriterio(Registro *r, char *campo, char *valorStr, int valorInt, int isNulo);

// Lê os campos de um novo Registro a partir da entrada
void leRegistroTeclado(Registro *r);

// Atualiza um campo específico do Registro com o novo valor fornecido
void atualizaCampo(Registro *r, char *campo, char *valorStr, int valorInt, int isNulo);

// Verifica se um campo é do tipo string
int campoString(char *campo);

// Lê os critérios de busca a partir da entrada, preenchendo o array de CampoBusca
void lerCriteriosBusca(CampoBusca criterios[], int qtdCriterios, int *possuiCodEstacao, int *valorCodEstacao);

// Verifica se o Registro satisfaz todos os critérios de busca fornecidos
int registroSatisfazCriterios(Registro *r, CampoBusca criterios[], int qtdCriterios);

#endif

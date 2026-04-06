#ifndef REGISTRO_H
#define REGISTRO_H
#include <stdio.h>

#define TAM_REGISTRO 80

//define as operações principais que são usadas várias vezes sobre os registros


//struct com os campos definidos para o registro
typedef struct {
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


void lerRegistroCSV(char *linha, Registro *r);
void escreveRegistroBin(FILE *bin, Registro *r);
int leRegistroBin(FILE *bin, Registro *r);
void imprimeRegistro(Registro *r);
int atendeCriterio(Registro *r, char *campo, char *valorStr, int valorInt, int isNulo);
void leRegistroTeclado(Registro *r);
void autualizaCampo(Registro *r, char *campo, char *valorStr, int valorInt, int isNulo);

#endif
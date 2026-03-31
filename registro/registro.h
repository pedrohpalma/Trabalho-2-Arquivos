#ifndef REGISTRO_H
#define REGISTRO_H

#include <stdio.h>
#include <stdlib.h>


typedef struct {
    char removido;
    int prox;

    int codestacao;
    int codlinha;
    int codproxestacao;
    int distproxestacao;
    int codlinhaintegra;
    int codestintegra;

    int tamNomeestacao;
    char nomeEstacao[50];

    int tamNomelinha;
    char nomelinha[50];

} Registro;


void initRegistro(Registro *r); 
Registro lerRegistroBin(FILE *fp);
void escreverRegistroBin(FILE *fp, Registro *r); 
int lerRegistroCSV(FILE *csv, Registro *r);
void printRegistro(Registro *r);

#endif
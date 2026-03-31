#include <stdio.h>
#include <stdlib.h>
#include "cabecalho.h"

void initCabecalho(Cabecalho *c) {
    c->status = '0';          // Ao abrir para escrita, status deve ser '0' (inconsistente) 
    c->topo = -1;             // -1 indica que não há registros logicamente removidos [cite: 50]
    c->proxRRN = 0;           // Deve ser iniciado com o valor 0 [cite: 56]
    c->nroEstacoes = 0;       // Inicia zerado
    c->nroParesEstacao = 0;   // Inicia zerado
}

void lerCabecalho(FILE *fp, Cabecalho *c) {
    // Garante que o ponteiro de leitura está no byte 0 do arquivo
    fseek(fp, 0, SEEK_SET);

    // Lê estritamente na ordem definida na representação gráfica [cite: 61, 64]
    fread(&c->status, sizeof(char), 1, fp);
    fread(&c->topo, sizeof(int), 1, fp);
    fread(&c->proxRRN, sizeof(int), 1, fp);
    fread(&c->nroEstacoes, sizeof(int), 1, fp);
    fread(&c->nroParesEstacao, sizeof(int), 1, fp);
}

void escreverCabecalho(FILE *fp, Cabecalho *c) {
    // Garante que o ponteiro de escrita está no byte 0 do arquivo
    fseek(fp, 0, SEEK_SET);

    // Escreve estritamente na ordem definida na representação gráfica [cite: 61, 64]
    fwrite(&c->status, sizeof(char), 1, fp);
    fwrite(&c->topo, sizeof(int), 1, fp);
    fwrite(&c->proxRRN, sizeof(int), 1, fp);
    fwrite(&c->nroEstacoes, sizeof(int), 1, fp);
    fwrite(&c->nroParesEstacao, sizeof(int), 1, fp);
}

void atualizarStatus(FILE *fp, char novoStatus) {
    // O status é o primeiríssimo byte do arquivo [cite: 61]
    fseek(fp, 0, SEEK_SET);
    fwrite(&novoStatus, sizeof(char), 1, fp);
    
    // Volta o ponteiro de arquivo para o início para não atrapalhar leituras/escritas subsequentes
    fseek(fp, 0, SEEK_SET); 
}
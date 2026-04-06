#include "cabecalho.h"

void inicializa_cabecalho(Cabecalho *c) {
    c->status = '0'; // status inicial (0 = sendo escrito)
    c->topo = -1;    // topo igual a -1 para ausência de registros removidos
    c->proxRRN = 0;
    c->nroEstacoes = 0;
    c->nroParesEstacao = 0;
}

void escreve_cabecalho(FILE *bin, Cabecalho *c) {
    // Escreve campo a campo de forma fixa sem padding extra, total = 17 bytes
    fwrite(&c->status, 1, 1, bin);
    fwrite(&c->topo, 4, 1, bin);
    fwrite(&c->proxRRN, 4, 1, bin);
    fwrite(&c->nroEstacoes, 4, 1, bin);
    fwrite(&c->nroParesEstacao, 4, 1, bin);
}

void atualiza_cabecalho(FILE *bin, Cabecalho *c) {
    c->status = '1'; // Finalizou a escrita com sucesso
    fseek(bin, 0, SEEK_SET);
    escreve_cabecalho(bin, c);
}

int le_cabecalho(FILE *bin, Cabecalho *c) {
    // Tenta ler o primeiro campo (status). Se não conseguir ler 1 byte, falhou
    if (fread(&c->status, 1, 1, bin) != 1) {
        return 0;
    }
    // Se o status for '0', o arquivo está inconsistente e não deve ser processado 
    if (c->status == '0') {
        return 0;
    }

    // Lê o restante dos campos sequencialmente, campo a campo 
    fread(&c->topo, 4, 1, bin);
    fread(&c->proxRRN, 4, 1, bin);
    fread(&c->nroEstacoes, 4, 1, bin);
    fread(&c->nroParesEstacao, 4, 1, bin);

    return 1;
}
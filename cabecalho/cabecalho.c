#include <string.h>
#include "cabecalho.h"
#include "../registro/registro.h"


// Define os valores iniciais do cabecalho antes de criar um novo arquivo binario
void initCabecalho(Cabecalho *c) {
    c->status = '0'; // '0' indica que o arquivo esta sendo escrito (inconsistente)
    c->topo = -1;    // -1 indica pilha de removidos vazia
    c->proxRRN = 0;
    c->nroEstacoes = 0;
    c->nroParesEstacao = 0;
}


// Serializa e grava os campos do cabecalho na posicao atual do arquivo binario
void escreveCabecalho(FILE *bin, Cabecalho *c) {
    fwrite(&c->status, 1, 1, bin);
    fwrite(&c->topo, 4, 1, bin);
    fwrite(&c->proxRRN, 4, 1, bin);
    fwrite(&c->nroEstacoes, 4, 1, bin);
    fwrite(&c->nroParesEstacao, 4, 1, bin);
}


// Define status='1' (consistente) e reescreve o cabecalho no inicio do arquivo
void atualizaCabecalho(FILE *bin, Cabecalho *c) {
    c->status = '1';
    fseek(bin, 0, SEEK_SET);
    escreveCabecalho(bin, c);
}


// Le o cabecalho do arquivo binario; retorna 0 se falhar ou se o arquivo estiver inconsistente
int leCabecalho(FILE *bin, Cabecalho *c) {
    // Tenta ler o campo 'status' (1 byte); falha indica arquivo vazio ou ilegivel
    if (fread(&c->status, 1, 1, bin) != 1) {
        return 0;
    }
    // Status '0' indica que o arquivo foi interrompido durante uma escrita anterior
    if (c->status == '0') {
        return 0;
    }

    // Le os demais campos sequencialmente
    fread(&c->topo, 4, 1, bin);
    fread(&c->proxRRN, 4, 1, bin);
    fread(&c->nroEstacoes, 4, 1, bin);
    fread(&c->nroParesEstacao, 4, 1, bin);

    return 1;
}


// Percorre todos os registros ativos e recontaliza nroEstacoes e nroParesEstacao no cabecalho
void atualizaContagemEstacoes(FILE *bin, Cabecalho *c) {
    c->nroEstacoes = 0;
    c->nroParesEstacao = 0;
    char estacoes_unicas[2000][100];
    int pares_unicos[2000][2];

    fseek(bin, TAM_CABECALHO, SEEK_SET);
    Registro reg;
    int status;
    
    while ((status = leRegistroBin(bin, &reg)) != 0) {
        if (status == 2) continue; // pula registros logicamente removidos

        // Adiciona nomeEstacao ao conjunto de unicos se ainda nao estiver presente
        int estacao_existe = 0;
        for (int k = 0; k < c->nroEstacoes; k++) {
            if (strcmp(estacoes_unicas[k], reg.nomeEstacao) == 0) {
                estacao_existe = 1; break;
            }
        }
        if (!estacao_existe) {
            strcpy(estacoes_unicas[c->nroEstacoes], reg.nomeEstacao);
            c->nroEstacoes++;
        }

        // Adiciona o par (codEstacao, codProxEstacao) se ainda nao estiver presente e nao for nulo
        if (reg.codProxEstacao != -1) {
            int par_existe = 0;
            for (int k = 0; k < c->nroParesEstacao; k++) {
                if (pares_unicos[k][0] == reg.codEstacao && pares_unicos[k][1] == reg.codProxEstacao) {
                    par_existe = 1; break;
                }
            }
            if (!par_existe) {
                pares_unicos[c->nroParesEstacao][0] = reg.codEstacao;
                pares_unicos[c->nroParesEstacao][1] = reg.codProxEstacao;
                c->nroParesEstacao++;
            }
        }
    }
}

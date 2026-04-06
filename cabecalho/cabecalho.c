#include <string.h>
#include "cabecalho.h"
#include "../registro/registro.h"


//inicia o cabecalho zerado,
void initCabecalho(Cabecalho *c) {
    c->status = '0'; // status inicial (0 = sendo escrito)
    c->topo = -1;    // topo igual a -1 para ausência de registros removidos
    c->proxRRN = 0;
    c->nroEstacoes = 0;
    c->nroParesEstacao = 0;
}


//recebe as informações atualizadas do cabeçalho c e atualiza no arquivo
void escreveCabecalho(FILE *bin, Cabecalho *c) {
    fwrite(&c->status, 1, 1, bin);
    fwrite(&c->topo, 4, 1, bin);
    fwrite(&c->proxRRN, 4, 1, bin);
    fwrite(&c->nroEstacoes, 4, 1, bin);
    fwrite(&c->nroParesEstacao, 4, 1, bin);
}

//muda status do cabecalho quando termina escrita do sucesso
void atualizaCabecalho(FILE *bin, Cabecalho *c) {
    c->status = '1';
    fseek(bin, 0, SEEK_SET);
    escreveCabecalho(bin, c);
}

//salva info do cabeçalho em struct
int leCabecalho(FILE *bin, Cabecalho *c) {
    // tenta ler o primeiro campo (status). Se nao conseguir ler 1 byte, falhou
    if (fread(&c->status, 1, 1, bin) != 1) {
        return 0;
    }
    // se o status for 0, o arquivo está inconsistente e não deve ser processado 
    if (c->status == '0') {
        return 0;
    }

    // Le o restante dos campos sequencialmente, campo a campo 
    fread(&c->topo, 4, 1, bin);
    fread(&c->proxRRN, 4, 1, bin);
    fread(&c->nroEstacoes, 4, 1, bin);
    fread(&c->nroParesEstacao, 4, 1, bin);

    return 1;
}


//percorre o arquivo novamente e atualiza o cabecalho quando existem modificaçoes, recontando o n de estacoes e n de pares
void atualizaContagemEstacoes(FILE *bin, Cabecalho *c) {
    c->nroEstacoes = 0;
    c->nroParesEstacao = 0;
    char estacoes_unicas[2000][100];
    int pares_unicos[2000][2];

    fseek(bin, 17, SEEK_SET);
    Registro reg;
    int status;
    
    while ((status = leRegistroBin(bin, &reg)) != 0) {
        if (status == 2) continue; // Pula removidos

        // Checagem de Estação Única
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

        // Checagem de Par Único
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
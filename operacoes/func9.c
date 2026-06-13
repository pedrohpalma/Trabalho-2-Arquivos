#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func9.h"
#include "../arquivos/arquivos.h"
#include "../cabecalho/cabecalho.h"
#include "../registro/registro.h"
#include "../utilitarios/utils.h"
#include "../arvoreB/arvoreB.h"
#include "../cabecalhoArvoreB/cabecalhoArvoreB.h"

// Funcao Auxiliar: Insere um registro no arquivo de dados.
// Estrategia: Tenta reaproveitar espaco de registros logicamente removidos (gerenciados via pilha no cabecalho).
// Se a pilha estiver vazia (topo == -1), anexa no final do arquivo (proxRRN).
static long inserirRegistroDadosFunc9(FILE *bin, Cabecalho *c, Registro *r) {
    long offset;

    // Caso 1: Reuso de espaco (Pilha de removidos NAO esta vazia)
    if (c->topo != -1) {
        int rrn_reuso = c->topo;
        int proximo_removido;

        offset = TAM_CABECALHO + (rrn_reuso * TAM_REGISTRO);
        
        // Pula o byte de status 'removido' ('1') para ler o RRN do proximo item da pilha
        fseek(bin, offset + 1, SEEK_SET);

        // Le qual sera o novo topo da pilha apos consumirmos este espaco
        if (fread(&proximo_removido, 4, 1, bin) != 1) {
            return -1; // Erro de leitura
        }

        // Atualiza metadados e sobrescreve o registro removido com o novo registro
        c->topo = proximo_removido;
        fseek(bin, offset, SEEK_SET);
        escreveRegistroBin(bin, r);
    } 
    // Caso 2: Insercao no fim do arquivo (Nenhum espaco para reuso)
    else {
        offset = TAM_CABECALHO + (c->proxRRN * TAM_REGISTRO);
        fseek(bin, offset, SEEK_SET);
        escreveRegistroBin(bin, r);
        c->proxRRN++;
    }

    // Salva o cabecalho atualizado imediatamente para manter sincronia
    fseek(bin, 0, SEEK_SET);
    escreveCabecalho(bin, c);
    fflush(bin);

    return offset; // Retorna o byteOffset exato onde o dado foi inserido (necessario para a Arvore-B)
}


// Funcao Insert Into: Insere 'n' novos registros no arquivo de dados e suas chaves no indice Arvore-B
void func9(char *arqEntrada, char *arqIndice, int n) {
    FILE *bin = abrirArquivo(arqEntrada, "rb+");
    if (!bin) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    FILE *indice = abrirArquivo(arqIndice, "rb+");
    if (!indice) {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        return;
    }

    Cabecalho c;
    if (!leCabecalho(bin, &c)) {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        fecharArquivo(indice);
        return;
    }

    CabecalhoArvoreB cabecalhoIndice;
    // Verifica a consistencia dos dois arquivos (dados e indice) antes de alterar qualquer coisa
    if (!lerCabecalhoArvoreB(indice, &cabecalhoIndice) || cabecalhoIndice.status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        fecharArquivo(indice);
        return;
    }

    Registro registros[n];
    int deveInserir[n];
    
    // Etapa 1: Leitura em memoria e Validacao de Chaves Primarias (codEstacao)
    for (int i = 0; i < n; i++) {
        int referenciaExistente;

        leRegistroTeclado(&registros[i]);
        deveInserir[i] = 1; // Assume inicialmente que e seguro inserir

        // Validacao A: Checa se o codigo ja existe no arquivo de dados (consultando o Indice)
        if (buscarArvoreB(indice, registros[i].codEstacao, &referenciaExistente)) {
            deveInserir[i] = 0;
        }

        // Validacao B: Checa duplicidade DENTRO do proprio lote de novos registros recebidos
        for (int j = 0; j < i; j++) {
            if (deveInserir[j] && registros[j].codEstacao == registros[i].codEstacao) {
                deveInserir[i] = 0;
            }
        }
    }

    // Marca ambos os arquivos como inconsistentes ('0') para iniciar a bateria de insercoes fisicas
    c.status = '0';
    fseek(bin, 0, SEEK_SET);
    escreveCabecalho(bin, &c);

    cabecalhoIndice.status = '0';
    if (!escreverCabecalhoArvoreB(indice, &cabecalhoIndice)) {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        fecharArquivo(indice);
        return;
    }

    // Etapa 2: Insercao Fisica (Dados + Indice)
    for (int i = 0; i < n; i++) {
        long offsetInserido;

        // Pula os registros marcados como duplicados na Etapa 1
        if (!deveInserir[i]) {
            continue;
        }

        // Insere no arquivo .bin
        offsetInserido = inserirRegistroDadosFunc9(bin, &c, &registros[i]);
        if (offsetInserido == -1) {
            printf("Falha no processamento do arquivo.\n");
            fecharArquivo(bin);
            fecharArquivo(indice);
            return;
        }

        // Insere a chave primaria e o offset gerado na Arvore-B (.idx)
        if (!inserirArvoreB(indice, registros[i].codEstacao, (int)offsetInserido)) {
            printf("Falha no processamento do arquivo.\n");
            fecharArquivo(bin);
            fecharArquivo(indice);
            return;
        }
    }

    // Etapa 3: Finalizacao e Consolidacao
    // Reconta estacoes para garantir que 'nroEstacoes' reflita o novo total
    atualizaContagemEstacoes(bin, &c);
    atualizaCabecalho(bin, &c);

    // Restaura o status do indice Arvore-B para consistente ('1')
    if (!atualizarStatusArvoreB(indice, '1')) {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin);
        fecharArquivo(indice);
        return;
    }

    fecharArquivo(bin);
    fecharArquivo(indice);

    // Chama o utilitario para validacao dos testes automatizados
    BinarioNaTela(arqEntrada);
    BinarioNaTela(arqIndice);
}
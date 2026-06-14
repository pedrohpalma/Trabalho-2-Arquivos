#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func10.h"
#include "../arquivos/arquivos.h"
#include "../cabecalho/cabecalho.h"
#include "../registro/registro.h"
#include "../utilitarios/utils.h"
#include "../arvoreB/arvoreB.h"
#include "../cabecalhoArvoreB/cabecalhoArvoreB.h"

// armazena temporariamente dados dos registros a serem removidos para evitar conflitos durante a varredura
typedef struct {
    long byteOffset;
    int codEstacao;
} RegistroRemocao;

// gerencia a alocacao dinamica do vetor auxiliar de remocoes, dobrando sua capacidade quando enche
static int adicionarRemocao(RegistroRemocao **remocoes, int *qtdRemocoes, int *capacidade, long byteOffset, int codEstacao) {
    if (*qtdRemocoes >= *capacidade) {
        int novaCapacidade = (*capacidade == 0) ? 8 : (*capacidade * 2);
        RegistroRemocao *novo = realloc(*remocoes, novaCapacidade * sizeof(RegistroRemocao));
        if (!novo) return 0;
        *remocoes = novo;
        *capacidade = novaCapacidade;
    }

    (*remocoes)[*qtdRemocoes].byteOffset = byteOffset;
    (*remocoes)[*qtdRemocoes].codEstacao = codEstacao;
    (*qtdRemocoes)++;
    return 1;
}

// encontra os registros que satisfazem os criterios e salva suas posicoes no vetor auxiliar de remocoes
static int buscarRegistrosParaRemocao(FILE *bin, FILE *indice, CampoBusca criterios[], int qtdCriterios, int possuiCodEstacao, int valorCodEstacao, RegistroRemocao **remocoes, int *qtdRemocoes) {
    int capacidade = 0;
    *remocoes = NULL;
    *qtdRemocoes = 0;

    // faz a busca otimizada pela arvoreB se o codigo da estacao for um dos criterios
    if (possuiCodEstacao) {
        int referencia;
        Registro r;
        int status_leitura;

        if (!buscarArvoreB(indice, valorCodEstacao, &referencia)) return 1;

        fseek(bin, referencia, SEEK_SET);
        status_leitura = leRegistroBin(bin, &r);

        // garante que o registro achado pela chave tambem cumpra todos os outros criterios passados
        if (status_leitura == 1 && registroSatisfazCriterios(&r, criterios, qtdCriterios)) {
            if (!adicionarRemocao(remocoes, qtdRemocoes, &capacidade, referencia, r.codEstacao)) return 0;
        }
        return 1;
    }

    // realiza busca sequencial no arquivo todo caso nao tenha o codigo da estacao como criterio
    fseek(bin, TAM_CABECALHO, SEEK_SET);
    Registro r;
    int status_leitura;

    while (1) {
        // grava o byte offset exato de onde comeca o registro antes de fazer a leitura
        long offsetRegistro = ftell(bin);
        status_leitura = leRegistroBin(bin, &r);

        if (status_leitura == 0) break;
        if (status_leitura == 2) continue;

        if (registroSatisfazCriterios(&r, criterios, qtdCriterios)) {
            if (!adicionarRemocao(remocoes, qtdRemocoes, &capacidade, offsetRegistro, r.codEstacao)) return 0;
        }
    }
    return 1;
}

// marca um registro como removido logicamente ('1') e o empilha no cabecalho para futuro reaproveitamento
static int removerLogicamenteRegistroDados(FILE *bin, Cabecalho *c, long byteOffset) {
    char removido;
    int rrn = (byteOffset - TAM_CABECALHO) / TAM_REGISTRO;
    int topoAntigo = c->topo;

    fseek(bin, byteOffset, SEEK_SET);
    if (fread(&removido, 1, 1, bin) != 1) return 0;

    if (removido == '1') return 1;

    removido = '1';
    fseek(bin, byteOffset, SEEK_SET);
    
    // escreve o status de removido e o rrn do topo antigo para manter a ligacao da pilha
    if (fwrite(&removido, 1, 1, bin) != 1) return 0;
    if (fwrite(&topoAntigo, 4, 1, bin) != 1) return 0;

    // atualiza cabecalho para que o topo aponte para o registro recem-removido
    c->topo = rrn;
    fseek(bin, 0, SEEK_SET);
    escreveCabecalho(bin, c);
    fflush(bin);

    return 1;
}

// coordena a remocao de registros, apagando no arquivo de dados e removendo as chaves da arvoreB
void func10(char *arqEntrada, char *arqIndice, int n) {
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
        fecharArquivo(bin); fecharArquivo(indice);
        return;
    }

    CabecalhoArvoreB cabecalhoIndice;
    // valida se o arquivo de indice esta integro e pronto para uso
    if (!lerCabecalhoArvoreB(indice, &cabecalhoIndice) || cabecalhoIndice.status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin); fecharArquivo(indice);
        return;
    }

    // muda o status de ambos os arquivos para inconsistente durante operacoes de remocao
    c.status = '0';
    fseek(bin, 0, SEEK_SET);
    escreveCabecalho(bin, &c);

    cabecalhoIndice.status = '0';
    if (!escreverCabecalhoArvoreB(indice, &cabecalhoIndice)) {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin); fecharArquivo(indice);
        return;
    }

    for (int i = 0; i < n; i++) {
        int m;
        scanf("%d", &m);

        CampoBusca criterios[m];
        int possuiCodEstacao, valorCodEstacao;
        RegistroRemocao *remocoes = NULL;
        int qtdRemocoes = 0;

        lerCriteriosBusca(criterios, m, &possuiCodEstacao, &valorCodEstacao);

        // passo 1: preenche o vetor com todos os registros que deram match com a busca
        if (!buscarRegistrosParaRemocao(bin, indice, criterios, m, possuiCodEstacao, valorCodEstacao, &remocoes, &qtdRemocoes)) {
            free(remocoes);
            printf("Falha no processamento do arquivo.\n");
            fecharArquivo(bin); fecharArquivo(indice);
            return;
        }

        // passo 2: percorre o vetor e efetiva a delecao no binario e no indice arvoreB
        for (int j = 0; j < qtdRemocoes; j++) {
            if (!removerLogicamenteRegistroDados(bin, &c, remocoes[j].byteOffset)) {
                free(remocoes);
                printf("Falha no processamento do arquivo.\n");
                fecharArquivo(bin); fecharArquivo(indice);
                return;
            }

            if (!removerArvoreB(indice, remocoes[j].codEstacao)) {
                free(remocoes);
                printf("Falha no processamento do arquivo.\n");
                fecharArquivo(bin); fecharArquivo(indice);
                return;
            }
        }
        free(remocoes);
    }

    // atualiza informacoes globais e devolve a consistencia aos arquivos salvos
    atualizaContagemEstacoes(bin, &c);
    atualizaCabecalho(bin, &c);

    if (!atualizarStatusArvoreB(indice, '1')) {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin); fecharArquivo(indice);
        return;
    }

    fecharArquivo(bin);
    fecharArquivo(indice);

    BinarioNaTela(arqEntrada);
    BinarioNaTela(arqIndice);
}
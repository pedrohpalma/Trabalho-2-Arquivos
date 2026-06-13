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

// Estrutura para armazenar temporariamente os registros que devem ser removidos.
// Isso evita corromper a leitura enquanto o arquivo esta sendo varrido.
typedef struct {
    long byteOffset;
    int codEstacao;
} RegistroRemocao;

// Funcao Auxiliar: Gerencia a memoria dinamica do array de remocoes (realoca dobrando a capacidade quando necessario)
static int adicionarRemocao(RegistroRemocao **remocoes, int *qtdRemocoes, int *capacidade, long byteOffset, int codEstacao) {
    if (*qtdRemocoes >= *capacidade) {
        int novaCapacidade = (*capacidade == 0) ? 8 : (*capacidade * 2);
        RegistroRemocao *novo = realloc(*remocoes, novaCapacidade * sizeof(RegistroRemocao));
        if (!novo) return 0; // Falha de alocacao
        *remocoes = novo;
        *capacidade = novaCapacidade;
    }

    (*remocoes)[*qtdRemocoes].byteOffset = byteOffset;
    (*remocoes)[*qtdRemocoes].codEstacao = codEstacao;
    (*qtdRemocoes)++;
    return 1;
}

// Funcao Auxiliar: Coleta offsets de todos os registros que deram "match" com os criterios
// Usa a Arvore-B se tiver o codEstacao, senao faz busca sequencial. Retorna 1 (Sucesso) ou 0 (Erro de memoria).
static int buscarRegistrosParaRemocao(FILE *bin, FILE *indice, CampoBusca criterios[], int qtdCriterios, int possuiCodEstacao, int valorCodEstacao, RegistroRemocao **remocoes, int *qtdRemocoes) {
    int capacidade = 0;
    *remocoes = NULL;
    *qtdRemocoes = 0;

    // Cenário 1: Busca otimizada via Arvore-B
    if (possuiCodEstacao) {
        int referencia;
        Registro r;
        int status_leitura;

        if (!buscarArvoreB(indice, valorCodEstacao, &referencia)) return 1; // Nao achou nada, retorna sucesso sem preencher remocoes

        fseek(bin, referencia, SEEK_SET);
        status_leitura = leRegistroBin(bin, &r);

        // Valida se o registro lido do indice satisfaz o RESTANTE dos criterios (ex: codEstacao = X AND nome = Y)
        if (status_leitura == 1 && registroSatisfazCriterios(&r, criterios, qtdCriterios)) {
            if (!adicionarRemocao(remocoes, qtdRemocoes, &capacidade, referencia, r.codEstacao)) return 0;
        }
        return 1;
    }

    // Cenário 2: Busca sequencial (fallback)
    fseek(bin, TAM_CABECALHO, SEEK_SET);
    Registro r;
    int status_leitura;

    while (1) {
        long offsetRegistro = ftell(bin); // Salva a posicao EXATA de onde comeca o registro atual
        status_leitura = leRegistroBin(bin, &r);

        if (status_leitura == 0) break; // EOF
        if (status_leitura == 2) continue; // Pula ja removidos

        if (registroSatisfazCriterios(&r, criterios, qtdCriterios)) {
            if (!adicionarRemocao(remocoes, qtdRemocoes, &capacidade, offsetRegistro, r.codEstacao)) return 0;
        }
    }
    return 1;
}

// Funcao Auxiliar: Efetua a remocao fisica alterando o byte '*' ou '1' e empilhando no topo
static int removerLogicamenteRegistroDados(FILE *bin, Cabecalho *c, long byteOffset) {
    char removido;
    int rrn = (byteOffset - TAM_CABECALHO) / TAM_REGISTRO; // Traduz byteOffset absoluto para RRN (indice relativo)
    int topoAntigo = c->topo;

    fseek(bin, byteOffset, SEEK_SET);
    if (fread(&removido, 1, 1, bin) != 1) return 0;

    // Se por acaso já estiver removido (caso edge), aborta silenciosamente
    if (removido == '1') return 1;

    removido = '1';
    fseek(bin, byteOffset, SEEK_SET); // Volta pro inicio do registro para sobrescrever
    
    // Escreve flag de remocao e atualiza o topo da pilha de removidos
    if (fwrite(&removido, 1, 1, bin) != 1) return 0;
    if (fwrite(&topoAntigo, 4, 1, bin) != 1) return 0;

    // Atualiza metadados do cabecalho com o novo topo
    c->topo = rrn;
    fseek(bin, 0, SEEK_SET);
    escreveCabecalho(bin, c);
    fflush(bin);

    return 1;
}

// Funcao Delete com Arvore-B: Coleta alvos, aplica remocao no arquivo de dados e exclui chaves do indice
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
    if (!lerCabecalhoArvoreB(indice, &cabecalhoIndice) || cabecalhoIndice.status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin); fecharArquivo(indice);
        return;
    }

    // Marca AMBOS os arquivos como inconsistentes (status '0') durante o processo destrutivo
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

        // Etapa 1: Coletar
        if (!buscarRegistrosParaRemocao(bin, indice, criterios, m, possuiCodEstacao, valorCodEstacao, &remocoes, &qtdRemocoes)) {
            free(remocoes);
            printf("Falha no processamento do arquivo.\n");
            fecharArquivo(bin); fecharArquivo(indice);
            return;
        }

        // Etapa 2: Aplicar
        for (int j = 0; j < qtdRemocoes; j++) {
            // Remove no .bin
            if (!removerLogicamenteRegistroDados(bin, &c, remocoes[j].byteOffset)) {
                free(remocoes);
                printf("Falha no processamento do arquivo.\n");
                fecharArquivo(bin); fecharArquivo(indice);
                return;
            }

            // Remove no Indice Arvore-B
            if (!removerArvoreB(indice, remocoes[j].codEstacao)) {
                free(remocoes);
                printf("Falha no processamento do arquivo.\n");
                fecharArquivo(bin); fecharArquivo(indice);
                return;
            }
        }
        free(remocoes); // Limpa o buffer de remocoes desta iteracao
    }

    // Reconta o numero de registros e valores unicos se sua especificacao exigir
    atualizaContagemEstacoes(bin, &c);
    atualizaCabecalho(bin, &c);

    // Atualiza o status do índice para consistente após as remoções
    if (!atualizarStatusArvoreB(indice, '1')) {
        printf("Falha no processamento do arquivo.\n");
        fecharArquivo(bin); fecharArquivo(indice);
        return;
    }

    fecharArquivo(bin);
    fecharArquivo(indice);

    // Imprime na tela o checksum dos arquivos resultantes
    BinarioNaTela(arqEntrada);
    BinarioNaTela(arqIndice);
}
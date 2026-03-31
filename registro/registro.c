#include <stdio.h>
#include <stdlib.h>

#include "registro.h"

#include "registro.h"
#include <string.h>


//Construtor Registro
void initRegistro(Registro *r) {
    r->removido = '0';     // Registro ativo por padrão
    r->prox = -1;          // -1 indica que não aponta para removidos
    
    // Campos de tamanho fixo inicializados com nulo (-1)
    r->codestacao = -1;
    r->codlinha = -1;
    r->codproxestacao = -1;
    r->distproxestacao = -1;
    r->codlinhaintegra = -1;
    r->codestintegra = -1;
    
    // Tamanhos variáveis inicializados em 0 (nulo)
    r->tamNomeestacao = 0;
    r->tamNomelinha = 0;
    
    memset(r->nomeEstacao, 0, 50);
    memset(r->nomelinha, 0, 50);
}

// Função auxiliar para ler strings separadas por vírgula no CSV
void lerCampoCSV(char **linha, char *campoDestino) {
    char *inicio = *linha;
    int i = 0;
    while (*inicio != ',' && *inicio != '\n' && *inicio != '\r' && *inicio != '\0') {
        campoDestino[i++] = *inicio;
        inicio++;
    }
    campoDestino[i] = '\0';
    
    if (*inicio == ',' || *inicio == '\n' || *inicio == '\r') {
        inicio++; // Pula o delimitador para a próxima leitura
    }
    *linha = inicio;
}


void escreverRegistroBin(FILE *fp, Registro *r) {
    // 1. Escreve campos de tamanho fixo na ordem exata
    fwrite(&r->removido, sizeof(char), 1, fp);
    fwrite(&r->prox, sizeof(int), 1, fp);
    fwrite(&r->codestacao, sizeof(int), 1, fp);
    fwrite(&r->codlinha, sizeof(int), 1, fp);
    fwrite(&r->codproxestacao, sizeof(int), 1, fp);
    fwrite(&r->distproxestacao, sizeof(int), 1, fp);
    fwrite(&r->codlinhaintegra, sizeof(int), 1, fp);
    fwrite(&r->codestintegra, sizeof(int), 1, fp);
    
    // 2. Escreve campos de tamanho variável (apenas o tamanho útil)
    fwrite(&r->tamNomeestacao, sizeof(int), 1, fp);
    if (r->tamNomeestacao > 0) {
        // Strings de tamanho variável não são finalizadas com \0 no arquivo
        fwrite(r->nomeEstacao, sizeof(char), r->tamNomeestacao, fp);
    }
    
    fwrite(&r->tamNomelinha, sizeof(int), 1, fp);
    if (r->tamNomelinha > 0) {
        fwrite(r->nomelinha, sizeof(char), r->tamNomelinha, fp);
    }
    
    // 3. Preenchimento de lixo (padding) para completar 80 bytes 
    // Campos fixos ocupam: 1(char) + 4*7(ints) = 29 bytes.
    // Variáveis ocupam: 4(tam1) + tamNomeestacao + 4(tam2) + tamNomelinha
    // Total = 37 + tamNomeestacao + tamNomelinha
    int bytesEscritos = 37 + r->tamNomeestacao + r->tamNomelinha;
    int bytesRestantes = 80 - bytesEscritos;
    
    char lixo = '$';
    for (int i = 0; i < bytesRestantes; i++) {
        fwrite(&lixo, sizeof(char), 1, fp);
    }
}

// Mudei o retorno para int para facilitar a verificação de EOF
int lerRegistroBin(FILE *fp, Registro *r) {
    initRegistro(r);
    
    // Tenta ler o primeiro byte. Se falhar, é o fim do arquivo.
    if (fread(&r->removido, sizeof(char), 1, fp) != 1) {
        return 0; // EOF alcançado
    }
    
    fread(&r->prox, sizeof(int), 1, fp);
    fread(&r->codestacao, sizeof(int), 1, fp);
    fread(&r->codlinha, sizeof(int), 1, fp);
    fread(&r->codproxestacao, sizeof(int), 1, fp);
    fread(&r->distproxestacao, sizeof(int), 1, fp);
    fread(&r->codlinhaintegra, sizeof(int), 1, fp);
    fread(&r->codestintegra, sizeof(int), 1, fp);
    
    fread(&r->tamNomeestacao, sizeof(int), 1, fp);
    if (r->tamNomeestacao > 0) {
        fread(r->nomeEstacao, sizeof(char), r->tamNomeestacao, fp);
        r->nomeEstacao[r->tamNomeestacao] = '\0'; // Adiciona \0 para uso em C
    }
    
    fread(&r->tamNomelinha, sizeof(int), 1, fp);
    if (r->tamNomelinha > 0) {
        fread(r->nomelinha, sizeof(char), r->tamNomelinha, fp);
        r->nomelinha[r->tamNomelinha] = '\0';
    }
    
    // Calcula quantos bytes de lixo precisam ser pulados
    int bytesLidos = 37 + r->tamNomeestacao + r->tamNomelinha;
    int bytesRestantes = 80 - bytesLidos;
    
    if (bytesRestantes > 0) {
        fseek(fp, bytesRestantes, SEEK_CUR); // Pula o lixo ($) até o próximo registro
    }
    
    return 1; // Sucesso
}

int lerRegistroCSV(FILE *csv, Registro *r) {
    char linha[200];
    if (fgets(linha, sizeof(linha), csv) == NULL) {
        return 0; // EOF
    }
    
    initRegistro(r);
    char *ptr = linha;
    char buffer[50];
    
    // 1. codEstacao
    lerCampoCSV(&ptr, buffer);
    if (strlen(buffer) > 0) r->codestacao = atoi(buffer);
    
    // 2. nomeEstacao
    lerCampoCSV(&ptr, buffer);
    if (strlen(buffer) > 0) {
        strcpy(r->nomeEstacao, buffer);
        r->tamNomeestacao = strlen(buffer);
    }
    
    // 3. codLinha
    lerCampoCSV(&ptr, buffer);
    if (strlen(buffer) > 0) r->codlinha = atoi(buffer);
    
    // 4. nomeLinha
    lerCampoCSV(&ptr, buffer);
    if (strlen(buffer) > 0) {
        strcpy(r->nomelinha, buffer);
        r->tamNomelinha = strlen(buffer);
    }
    
    // 5. codProxEstacao
    lerCampoCSV(&ptr, buffer);
    if (strlen(buffer) > 0) r->codproxestacao = atoi(buffer);
    
    // 6. distProxEstacao
    lerCampoCSV(&ptr, buffer);
    if (strlen(buffer) > 0) r->distproxestacao = atoi(buffer);
    
    // 7. codLinhaIntegra
    lerCampoCSV(&ptr, buffer);
    if (strlen(buffer) > 0) r->codlinhaintegra = atoi(buffer);
    
    // 8. codEstIntegra
    lerCampoCSV(&ptr, buffer);
    if (strlen(buffer) > 0) r->codestintegra = atoi(buffer);
    
    return 1; // Sucesso
}

void printRegistro(Registro *r) {
    if (r->removido == '1') return; // Ignora registros logicamente removidos

    // Formatação conforme especificado: exibe o valor ou "NULO"
    printf("%d ", r->codestacao);
    
    if (r->tamNomeestacao > 0) printf("%s ", r->nomeEstacao);
    else printf("NULO ");
    
    if (r->codlinha != -1) printf("%d ", r->codlinha);
    else printf("NULO ");

    if (r->tamNomelinha > 0) printf("%s ", r->nomelinha);
    else printf("NULO ");

    if (r->codproxestacao != -1) printf("%d ", r->codproxestacao);
    else printf("NULO ");

    if (r->distproxestacao != -1) printf("%d ", r->distproxestacao);
    else printf("NULO ");

    if (r->codlinhaintegra != -1) printf("%d ", r->codlinhaintegra);
    else printf("NULO ");

    if (r->codestintegra != -1) printf("%d\n", r->codestintegra);
    else printf("NULO\n");
}
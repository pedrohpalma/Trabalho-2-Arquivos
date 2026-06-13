#include <string.h>
#include <stdlib.h>
#include "registro.h"
#include "../utilitarios/utils.h"


// Le uma linha do CSV e preenche todos os campos da struct Registro.
// Campos ausentes (celula vazia) sao armazenados como -1 (int) ou string vazia (string).
void lerRegistroCSV(char *linha, Registro *r){
    char *ptr = linha;
    char campo[256];

    // 1. codEstacao — obrigatorio, nunca nulo no CSV
    getProxCampo(&ptr, campo);
    r->codEstacao = atoi(campo);

    // 2. nomeEstacao — obrigatorio, nunca nulo no CSV
    getProxCampo(&ptr, campo);
    r->tamNomeEstacao = strlen(campo);
    strcpy(r->nomeEstacao, campo);

    // 3. codLinha — pode ser vazio (NULO = -1)
    getProxCampo(&ptr, campo);
    r->codLinha = (campo[0] == '\0' || campo[0] == ' ' || campo[0] == '\r' || campo[0] == '\n') ? -1 : atoi(campo);

    // 4. nomeLinha — pode ser vazio (NULO = string vazia, tamanho 0)
    getProxCampo(&ptr, campo);
    if (campo[0] == '\0' || campo[0] == ' ' || campo[0] == '\r' || campo[0] == '\n') {
        r->tamNomeLinha = 0;
        r->nomeLinha[0] = '\0';
    } else {
        r->tamNomeLinha = strlen(campo);
        strcpy(r->nomeLinha, campo);
    }

    // 5. codProxEstacao — pode ser vazio (NULO = -1)
    getProxCampo(&ptr, campo);
    r->codProxEstacao = (campo[0] == '\0' || campo[0] == ' ' || campo[0] == '\r' || campo[0] == '\n') ? -1 : atoi(campo);

    // 6. distProxEstacao — pode ser vazio (NULO = -1)
    getProxCampo(&ptr, campo);
    r->distProxEstacao = (campo[0] == '\0' || campo[0] == ' ' || campo[0] == '\r' || campo[0] == '\n') ? -1 : atoi(campo);

    // 7. codLinhaIntegra — pode ser vazio (NULO = -1)
    getProxCampo(&ptr, campo);
    r->codLinhaIntegra = (campo[0] == '\0' || campo[0] == ' ' || campo[0] == '\r' || campo[0] == '\n') ? -1 : atoi(campo);

    // 8. codEstIntegra — ultimo campo; pode trazer '\r' ou '\n' residual do fim de linha
    getProxCampo(&ptr, campo);
    if (campo[0] == '\0' || campo[0] == ' ' || campo[0] == '\r' || campo[0] == '\n') {
        r->codEstIntegra = -1;
    } else {
        r->codEstIntegra = atoi(campo);
    }

    // Campos de controle: registro ativo e sem proximo na pilha de removidos
    r->removido = '0';
    r->proximo = -1;
}


// Serializa o Registro no arquivo binario, garantindo exatamente TAM_REGISTRO (80) bytes.
// O espaco restante apos os campos e preenchido com '$' para manter o tamanho fixo.
void escreveRegistroBin(FILE *bin, Registro *r){
    fwrite(&r->removido, 1, 1, bin);
    fwrite(&r->proximo, 4, 1, bin);
    fwrite(&r->codEstacao, 4, 1, bin);
    fwrite(&r->codLinha, 4, 1, bin);
    fwrite(&r->codProxEstacao, 4, 1, bin);
    fwrite(&r->distProxEstacao, 4, 1, bin);
    fwrite(&r->codLinhaIntegra, 4, 1, bin);
    fwrite(&r->codEstIntegra, 4, 1, bin);
    
    // Campos de tamanho variavel: grava o tamanho seguido do conteudo
    fwrite(&r->tamNomeEstacao, 4, 1, bin);
    if (r->tamNomeEstacao > 0) {
        fwrite(r->nomeEstacao, 1, r->tamNomeEstacao, bin);
    }
    
    fwrite(&r->tamNomeLinha, 4, 1, bin);
    if (r->tamNomeLinha > 0) {
        fwrite(r->nomeLinha, 1, r->tamNomeLinha, bin);
    }

    // Calcula quantos bytes faltam para completar os 80 fixos e preenche com '$'
    // 37 = 1(removido) + 4(proximo) + 6*4(ints fixos) + 4(tamNome1) + 4(tamNome2)
    int bytes_escritos = 37 + r->tamNomeEstacao + r->tamNomeLinha;
    int lixo = TAM_REGISTRO - bytes_escritos;
    char cifrao = '$';
    for (int i = 0; i < lixo; i++) {
        fwrite(&cifrao, 1, 1, bin);
    }
}


// Le exatamente TAM_REGISTRO bytes do binario e desserializa para a struct Registro.
// Retorna: 0 = EOF, 1 = leitura normal, 2 = registro removido (pulado).
int leRegistroBin(FILE *bin, Registro *r){
    // Tenta ler o byte 'removido'; EOF indica fim dos registros
    if (fread(&r->removido, 1, 1, bin) != 1) {
        return 0;
    }
    
    // Registro removido: pula os 79 bytes restantes sem decodificar
    if (r->removido == '1') {
        fseek(bin, 79, SEEK_CUR);
        return 2;
    }
    
    // Registro ativo: le os 79 bytes restantes em um buffer e desserializa
    char buffer[79];
    fread(buffer, 1, 79, bin);
    
    int pos = 0;
    
    // Campos de tamanho fixo (4 bytes cada)
    memcpy(&r->proximo, &buffer[pos], 4); pos += 4;
    memcpy(&r->codEstacao, &buffer[pos], 4); pos += 4;
    memcpy(&r->codLinha, &buffer[pos], 4); pos += 4;
    memcpy(&r->codProxEstacao, &buffer[pos], 4); pos += 4;
    memcpy(&r->distProxEstacao, &buffer[pos], 4); pos += 4;
    memcpy(&r->codLinhaIntegra, &buffer[pos], 4); pos += 4;
    memcpy(&r->codEstIntegra, &buffer[pos], 4); pos += 4;
    
    // Campos de tamanho variavel: le o tamanho e depois a string
    memcpy(&r->tamNomeEstacao, &buffer[pos], 4); pos += 4;
    if (r->tamNomeEstacao > 0) {
        memcpy(r->nomeEstacao, &buffer[pos], r->tamNomeEstacao);
    }
    r->nomeEstacao[r->tamNomeEstacao] = '\0'; // garante terminador de string
    pos += r->tamNomeEstacao;
    
    memcpy(&r->tamNomeLinha, &buffer[pos], 4); pos += 4;
    if (r->tamNomeLinha > 0) {
        memcpy(r->nomeLinha, &buffer[pos], r->tamNomeLinha);
    }
    r->nomeLinha[r->tamNomeLinha] = '\0';
    
    return 1;
}


// Imprime todos os campos do Registro, substituindo valores ausentes por "NULO"
void imprimeRegistro(Registro *r){
    printf("%d ", r->codEstacao);
    
    if (r->tamNomeEstacao > 0) printf("%s ", r->nomeEstacao);
    else printf("NULO ");
    
    if (r->codLinha == -1) printf("NULO ");
    else printf("%d ", r->codLinha);
    
    if (r->tamNomeLinha == 0) printf("NULO ");
    else printf("%s ", r->nomeLinha);
    
    if (r->codProxEstacao == -1) printf("NULO ");
    else printf("%d ", r->codProxEstacao);
    
    if (r->distProxEstacao == -1) printf("NULO ");
    else printf("%d ", r->distProxEstacao);
    
    if (r->codLinhaIntegra == -1) printf("NULO ");
    else printf("%d ", r->codLinhaIntegra);
    
    if (r->codEstIntegra == -1) printf("NULO\n");
    else printf("%d\n", r->codEstIntegra);
}


// Verifica se o Registro satisfaz um criterio de busca (campo == valor).
// Trata tanto campos inteiros quanto strings, alem de buscas por NULO.
// Retorna 1 se o criterio e atendido, 0 caso contrario ou campo desconhecido.
int atendeCriterio(Registro *r, char *campo, char *valorStr, int valorInt, int isNulo){
    if (strcmp(campo, "codEstacao") == 0) {
        return isNulo ? (r->codEstacao == -1) : (r->codEstacao == valorInt);
    } 
    else if (strcmp(campo, "codLinha") == 0) {
        return isNulo ? (r->codLinha == -1) : (r->codLinha == valorInt);
    }
    else if (strcmp(campo, "codProxEstacao") == 0) {
        return isNulo ? (r->codProxEstacao == -1) : (r->codProxEstacao == valorInt);
    }
    else if (strcmp(campo, "distProxEstacao") == 0) {
        return isNulo ? (r->distProxEstacao == -1) : (r->distProxEstacao == valorInt);
    }
    // Aceita tanto "codLinhaIntegra" quanto a variante com 'l' minusculo (typo historico do CSV)
    else if (strcmp(campo, "codLinhaIntegra") == 0 || strcmp(campo, "codLinhalntegra") == 0) {
        return isNulo ? (r->codLinhaIntegra == -1) : (r->codLinhaIntegra == valorInt);
    }
    else if (strcmp(campo, "codEstIntegra") == 0 || strcmp(campo, "codEstacaoIntegra") == 0) {
        return isNulo ? (r->codEstIntegra == -1) : (r->codEstIntegra == valorInt);
    }
    else if (strcmp(campo, "nomeEstacao") == 0) {
        return isNulo ? (r->tamNomeEstacao == 0) : (strcmp(r->nomeEstacao, valorStr) == 0);
    }
    else if (strcmp(campo, "nomeLinha") == 0) {
        return isNulo ? (r->tamNomeLinha == 0) : (strcmp(r->nomeLinha, valorStr) == 0);
    }
    return 0; // campo nao reconhecido: criterio nunca e atendido
}


// Le os campos de um novo Registro a partir da entrada padrao.
// Strings sao lidas com ScanQuoteString; "NULO" e convertido para -1 (int) ou string vazia.
void leRegistroTeclado(Registro *r) {
    char buffer[256];

    // 1. codEstacao
    scanf("%s", buffer);
    r->codEstacao = (strcmp(buffer, "NULO") == 0) ? -1 : atoi(buffer);

    // 2. nomeEstacao
    ScanQuoteString(buffer);
    if (strcmp(buffer, "") == 0 || strcmp(buffer, "NULO") == 0) {
        r->tamNomeEstacao = 0;
        r->nomeEstacao[0] = '\0';
    } else {
        r->tamNomeEstacao = strlen(buffer);
        strcpy(r->nomeEstacao, buffer);
    }

    // 3. codLinha
    scanf("%s", buffer);
    r->codLinha = (strcmp(buffer, "NULO") == 0) ? -1 : atoi(buffer);

    // 4. nomeLinha
    ScanQuoteString(buffer);
    if (strcmp(buffer, "") == 0 || strcmp(buffer, "NULO") == 0) {
        r->tamNomeLinha = 0;
        r->nomeLinha[0] = '\0';
    } else {
        r->tamNomeLinha = strlen(buffer);
        strcpy(r->nomeLinha, buffer);
    }

    // 5. codProxEstacao
    scanf("%s", buffer);
    r->codProxEstacao = (strcmp(buffer, "NULO") == 0) ? -1 : atoi(buffer);

    // 6. distProxEstacao
    scanf("%s", buffer);
    r->distProxEstacao = (strcmp(buffer, "NULO") == 0) ? -1 : atoi(buffer);

    // 7. codLinhaIntegra
    scanf("%s", buffer);
    r->codLinhaIntegra = (strcmp(buffer, "NULO") == 0) ? -1 : atoi(buffer);

    // 8. codEstIntegra
    scanf("%s", buffer);
    r->codEstIntegra = (strcmp(buffer, "NULO") == 0) ? -1 : atoi(buffer);
    
    // Campos de controle: registro novo e sempre ativo e sem proximo na pilha
    r->removido = '0';
    r->proximo = -1;
}


// Atualiza um campo especifico do Registro com o novo valor fornecido.
// Para strings, trata o caso NULO (limpa o campo) e o caso normal (copia o novo valor).
void atualizaCampo(Registro *r, char *campo, char *valorStr, int valorInt, int isNulo) {
    if (strcmp(campo, "codEstacao") == 0) r->codEstacao = valorInt;
    else if (strcmp(campo, "codLinha") == 0) r->codLinha = valorInt;
    else if (strcmp(campo, "codProxEstacao") == 0) r->codProxEstacao = valorInt;
    else if (strcmp(campo, "distProxEstacao") == 0) r->distProxEstacao = valorInt;
    else if (strcmp(campo, "codLinhaIntegra") == 0 || strcmp(campo, "codLinhalntegra") == 0) r->codLinhaIntegra = valorInt;
    else if (strcmp(campo, "codEstIntegra") == 0 || strcmp(campo, "codEstacaoIntegra") == 0) r->codEstIntegra = valorInt;
    else if (strcmp(campo, "nomeEstacao") == 0) {
        if (isNulo) {
            r->tamNomeEstacao = 0;
            r->nomeEstacao[0] = '\0';
        } else {
            r->tamNomeEstacao = strlen(valorStr);
            strcpy(r->nomeEstacao, valorStr);
        }
    }
    else if (strcmp(campo, "nomeLinha") == 0) {
        if (isNulo) {
            r->tamNomeLinha = 0;
            r->nomeLinha[0] = '\0';
        } else {
            r->tamNomeLinha = strlen(valorStr);
            strcpy(r->nomeLinha, valorStr);
        }
    }
}

// Função auxiliar para identificar se um campo é do tipo string (nomeEstacao ou nomeLinha)
int campoString(char *campo){
    return strcmp(campo, "nomeEstacao") == 0 || strcmp(campo, "nomeLinha") == 0;
}

// Função auxiliar para ler os critérios de busca ou atualização, identificando campos string e inteiros, e tratando o caso NULO.
void lerCriteriosBusca(CampoBusca criterios[], int qtdCriterios, int *possuiCodEstacao, int *valorCodEstacao){
    *possuiCodEstacao = 0;
    *valorCodEstacao = -1;

    for (int i = 0; i < qtdCriterios; i++){
        scanf("%s", criterios[i].nomeCampo);
        criterios[i].valorInt = -1;
        criterios[i].valorStr[0] = '\0';
        criterios[i].isNulo = 0;

        if (campoString(criterios[i].nomeCampo)){
            ScanQuoteString(criterios[i].valorStr);
            criterios[i].isNulo = (strcmp(criterios[i].valorStr, "") == 0 ||
                                   strcmp(criterios[i].valorStr, "NULO") == 0)
                                      ? 1
                                      : 0;
        }
        else{
            char temp[50];
            scanf("%s", temp);
            if (strcmp(temp, "NULO") == 0){
                criterios[i].isNulo = 1;
                criterios[i].valorInt = -1;
            }
            else{
                criterios[i].valorInt = atoi(temp);
            }
        }

        if (strcmp(criterios[i].nomeCampo, "codEstacao") == 0){
            *possuiCodEstacao = 1;
            *valorCodEstacao = criterios[i].valorInt;
        }
    }
}

// Verifica se o Registro satisfaz todos os criterios de busca fornecidos.
// Retorna 1 se o registro atende a todos os criterios, 0 caso contrario.
int registroSatisfazCriterios(Registro *r, CampoBusca criterios[], int qtdCriterios){
    if (r->removido == '1'){
        return 0;
    }

    for (int i = 0; i < qtdCriterios; i++){
        if (!atendeCriterio(r, criterios[i].nomeCampo, criterios[i].valorStr, criterios[i].valorInt, criterios[i].isNulo)){
            return 0;
        }
    }

    return 1;
}
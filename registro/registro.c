#include <string.h>
#include <stdlib.h>
#include "registro.h"
#include "../utilitarios/utils.h"

void lerRegistroCSV(char *linha, Registro *r) {
    char *ptr = linha;
    char campo[256];

    // 1. codEstacao (não aceita nulo)
    getProxCampo(&ptr, campo);
    r->codEstacao = atoi(campo);

    // 2. nomeEstacao (não aceita nulo)
    getProxCampo(&ptr, campo);
    r->tamNomeEstacao = strlen(campo);
    strcpy(r->nomeEstacao, campo);

    // 3. codLinha
    getProxCampo(&ptr, campo);
    r->codLinha = (campo[0] == '\0' || campo[0] == ' ' || campo[0] == '\r' || campo[0] == '\n') ? -1 : atoi(campo);

    // 4. nomeLinha
    getProxCampo(&ptr, campo);
    if (campo[0] == '\0' || campo[0] == ' ' || campo[0] == '\r' || campo[0] == '\n') {
        r->tamNomeLinha = 0;
        r->nomeLinha[0] = '\0';
    } else {
        r->tamNomeLinha = strlen(campo);
        strcpy(r->nomeLinha, campo);
    }

    // 5. codProxEstacao
    getProxCampo(&ptr, campo);
    r->codProxEstacao = (campo[0] == '\0' || campo[0] == ' ' || campo[0] == '\r' || campo[0] == '\n') ? -1 : atoi(campo);

    // 6. distProxEstacao
    getProxCampo(&ptr, campo);
    r->distProxEstacao = (campo[0] == '\0' || campo[0] == ' ' || campo[0] == '\r' || campo[0] == '\n') ? -1 : atoi(campo);

    // 7. codLinhaIntegra
    getProxCampo(&ptr, campo);
    r->codLinhaIntegra = (campo[0] == '\0' || campo[0] == ' ' || campo[0] == '\r' || campo[0] == '\n') ? -1 : atoi(campo);

    // 8. codEstIntegra (O último campo, que costuma trazer o "Enter" escondido)
    getProxCampo(&ptr, campo);
    if (campo[0] == '\0' || campo[0] == ' ' || campo[0] == '\r' || campo[0] == '\n') {
        r->codEstIntegra = -1;
    } else {
        r->codEstIntegra = atoi(campo);
    }

    // Campos de controle
    r->removido = '0';
    r->proximo = -1;
}

void escreveRegistroBin(FILE *bin, Registro *r) {
    fwrite(&r->removido, 1, 1, bin);
    fwrite(&r->proximo, 4, 1, bin);
    fwrite(&r->codEstacao, 4, 1, bin);
    fwrite(&r->codLinha, 4, 1, bin);
    fwrite(&r->codProxEstacao, 4, 1, bin);
    fwrite(&r->distProxEstacao, 4, 1, bin);
    fwrite(&r->codLinhaIntegra, 4, 1, bin);
    fwrite(&r->codEstIntegra, 4, 1, bin);
    
    // Campos variáveis
    fwrite(&r->tamNomeEstacao, 4, 1, bin);
    if (r->tamNomeEstacao > 0) {
        fwrite(r->nomeEstacao, 1, r->tamNomeEstacao, bin);
    }
    
    fwrite(&r->tamNomeLinha, 4, 1, bin);
    if (r->tamNomeLinha > 0) {
        fwrite(r->nomeLinha, 1, r->tamNomeLinha, bin);
    }

    // Calcula lixo (tamanho máximo 80 bytes)
    int bytes_escritos = 37 + r->tamNomeEstacao + r->tamNomeLinha;
    int lixo = 80 - bytes_escritos;
    char cifrao = '$';
    
    for (int i = 0; i < lixo; i++) {
        fwrite(&cifrao, 1, 1, bin);
    }
}

// Lê exatamente 80 bytes do binário e decodifica para a struct Registro
int leRegistroBin(FILE *bin, Registro *r) {
    // Lê APENAS o campo 'removido' (1 byte)
    if (fread(&r->removido, 1, 1, bin) != 1) {
        return 0; // Retorna 0 quando chega no Fim do Arquivo (EOF)
    }
    
    // REGRA DO PROFESSOR: Pular registros removidos com fseek
    if (r->removido == '1') {
        fseek(bin, 79, SEEK_CUR); // Pula os 79 bytes restantes deste registro
        return 2; // Retorna o código 2 indicando "Registro Pulado"
    }
    
    // Se não está removido, lê os 79 bytes restantes
    char buffer[79];
    fread(buffer, 1, 79, bin);
    
    int pos = 0;
    
    // Lendo campos de tamanho fixo
    memcpy(&r->proximo, &buffer[pos], 4); pos += 4;
    memcpy(&r->codEstacao, &buffer[pos], 4); pos += 4;
    memcpy(&r->codLinha, &buffer[pos], 4); pos += 4;
    memcpy(&r->codProxEstacao, &buffer[pos], 4); pos += 4;
    memcpy(&r->distProxEstacao, &buffer[pos], 4); pos += 4;
    memcpy(&r->codLinhaIntegra, &buffer[pos], 4); pos += 4;
    memcpy(&r->codEstIntegra, &buffer[pos], 4); pos += 4;
    
    // Lendo campo variável: Nome da Estação
    memcpy(&r->tamNomeEstacao, &buffer[pos], 4); pos += 4;
    if (r->tamNomeEstacao > 0) {
        memcpy(r->nomeEstacao, &buffer[pos], r->tamNomeEstacao);
    }
    r->nomeEstacao[r->tamNomeEstacao] = '\0';
    pos += r->tamNomeEstacao;
    
    // Lendo campo variável: Nome da Linha
    memcpy(&r->tamNomeLinha, &buffer[pos], 4); pos += 4;
    if (r->tamNomeLinha > 0) {
        memcpy(r->nomeLinha, &buffer[pos], r->tamNomeLinha);
    }
    r->nomeLinha[r->tamNomeLinha] = '\0';
    
    return 1; // Retorna 1 indicando sucesso na leitura
}

void imprimeRegistro(Registro *r) {
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

int atendeCriterio(Registro *r, char *campo, char *valorStr, int valorInt, int isNulo) {
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
    // Suporte para eventuais erros de digitação do PDF (com 'l' no lugar do 'I' maiúsculo)
    else if (strcmp(campo, "codLinhaIntegra") == 0 || strcmp(campo, "codLinhalntegra") == 0) {
        return isNulo ? (r->codLinhaIntegra == -1) : (r->codLinhaIntegra == valorInt);
    }
    // CORREÇÃO: Aceita a variação do nome do campo usada nos casos de teste
    else if (strcmp(campo, "codEstIntegra") == 0 || strcmp(campo, "codEstacaoIntegra") == 0) {
        return isNulo ? (r->codEstIntegra == -1) : (r->codEstIntegra == valorInt);
    }
    else if (strcmp(campo, "nomeEstacao") == 0) {
        return isNulo ? (r->tamNomeEstacao == 0) : (strcmp(r->nomeEstacao, valorStr) == 0);
    }
    else if (strcmp(campo, "nomeLinha") == 0) {
        return isNulo ? (r->tamNomeLinha == 0) : (strcmp(r->nomeLinha, valorStr) == 0);
    }
    return 0; // Campo não reconhecido
}


// Lê dados do terminal e constrói o Registro
void leRegistroTeclado(Registro *r) {
    char buffer[256];

    // 1. codEstacao (int)
    scanf("%s", buffer);
    r->codEstacao = (strcmp(buffer, "NULO") == 0) ? -1 : atoi(buffer);

    // 2. nomeEstacao (string)
    ScanQuoteString(buffer);
    if (strcmp(buffer, "") == 0 || strcmp(buffer, "NULO") == 0) {
        r->tamNomeEstacao = 0;
        r->nomeEstacao[0] = '\0';
    } else {
        r->tamNomeEstacao = strlen(buffer);
        strcpy(r->nomeEstacao, buffer);
    }

    // 3. codLinha (int)
    scanf("%s", buffer);
    r->codLinha = (strcmp(buffer, "NULO") == 0) ? -1 : atoi(buffer);

    // 4. nomeLinha (string)
    ScanQuoteString(buffer);
    if (strcmp(buffer, "") == 0 || strcmp(buffer, "NULO") == 0) {
        r->tamNomeLinha = 0;
        r->nomeLinha[0] = '\0';
    } else {
        r->tamNomeLinha = strlen(buffer);
        strcpy(r->nomeLinha, buffer);
    }

    // 5. codProxEstacao (int)
    scanf("%s", buffer);
    r->codProxEstacao = (strcmp(buffer, "NULO") == 0) ? -1 : atoi(buffer);

    // 6. distProxEstacao (int)
    scanf("%s", buffer);
    r->distProxEstacao = (strcmp(buffer, "NULO") == 0) ? -1 : atoi(buffer);

    // 7. codLinhaIntegra (int)
    scanf("%s", buffer);
    r->codLinhaIntegra = (strcmp(buffer, "NULO") == 0) ? -1 : atoi(buffer);

    // 8. codEstIntegra (int)
    scanf("%s", buffer);
    r->codEstIntegra = (strcmp(buffer, "NULO") == 0) ? -1 : atoi(buffer);
    

    // Campos de controle lógicos para um novo registro
    r->removido = '0';
    r->proximo = -1;
}

// Atualiza um campo específico do registro com o novo valor
void autualizaCampo(Registro *r, char *campo, char *valorStr, int valorInt, int isNulo) {
    if (strcmp(campo, "codEstacao") == 0) r->codEstacao = valorInt;
    else if (strcmp(campo, "codLinha") == 0) r->codLinha = valorInt;
    else if (strcmp(campo, "codProxEstacao") == 0) r->codProxEstacao = valorInt;
    else if (strcmp(campo, "distProxEstacao") == 0) r->distProxEstacao = valorInt;
    
    // Suporte para eventuais erros de digitação do PDF
    else if (strcmp(campo, "codLinhaIntegra") == 0 || strcmp(campo, "codLinhalntegra") == 0) r->codLinhaIntegra = valorInt;
    
    // CORREÇÃO: Aceita a variação do nome do campo usada nos casos de teste
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
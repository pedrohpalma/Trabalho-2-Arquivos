#include <string.h>
#include <stdlib.h>
#include "registro.h"
#include "../utilitarios/utils.h"

void parse_linha_csv(char *linha, Registro *r) {
    char *ptr = linha;
    char campo[256];

    // codEstacao (não aceita nulo)
    get_next_field(&ptr, campo);
    r->codEstacao = atoi(campo);

    // nomeEstacao (não aceita nulo)
    get_next_field(&ptr, campo);
    r->tamNomeEstacao = strlen(campo);
    strcpy(r->nomeEstacao, campo);

    // codLinha
    get_next_field(&ptr, campo);
    r->codLinha = (campo[0] == '\0') ? -1 : atoi(campo);

    // nomeLinha
    get_next_field(&ptr, campo);
    r->tamNomeLinha = strlen(campo);
    strcpy(r->nomeLinha, campo);

    // codProxEstacao
    get_next_field(&ptr, campo);
    r->codProxEstacao = (campo[0] == '\0') ? -1 : atoi(campo);

    // distProxEstacao
    get_next_field(&ptr, campo);
    r->distProxEstacao = (campo[0] == '\0') ? -1 : atoi(campo);

    // codLinhaIntegra
    get_next_field(&ptr, campo);
    r->codLinhaIntegra = (campo[0] == '\0') ? -1 : atoi(campo);

    // codEstIntegra
    get_next_field(&ptr, campo);
    r->codEstIntegra = (campo[0] == '\0') ? -1 : atoi(campo);

    // Campos de controle
    r->removido = '0';
    r->proximo = -1;
}

void escreve_registro_bin(FILE *bin, Registro *r) {
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
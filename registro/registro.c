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

// Lê exatamente 80 bytes do binário e decodifica para a struct Registro
// Lê o registro do binário otimizando pulos de registros deletados
int le_registro_bin(FILE *bin, Registro *r) {
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
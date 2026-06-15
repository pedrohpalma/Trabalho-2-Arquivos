#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

// Extrai o proximo campo CSV de '*line_ptr', copiando para 'field' e avancando o ponteiro alem do separador
// Implementado manualmente para evitar problemas de estado global do scanf
void getProxCampo(char **line_ptr, char *field){
    char *p = *line_ptr;
    int i = 0;
    // Copia caracteres ate encontrar virgula, nova linha ou fim de string
    while (*p && *p != ',' && *p != '\n' && *p != '\r'){
        field[i++] = *p++;
    }
    field[i] = '\0';
    if (*p == ','){
        p++; // consome o separador para posicionar no inicio do proximo campo
    }
    *line_ptr = p;
}


// Funcao fornecida: calcula e imprime o checksum do arquivo binario
void BinarioNaTela(char *arquivo){
    FILE *fs;
    if (arquivo == NULL || !(fs = fopen(arquivo, "rb"))){
        fprintf(stderr,
                "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): "
                "não foi possível abrir o arquivo que me passou para leitura.\n");
        return;
    }
    fseek(fs, 0, SEEK_END);
    size_t fl = ftell(fs);
    fseek(fs, 0, SEEK_SET);
    unsigned char *mb = (unsigned char *)malloc(fl);
    fread(mb, 1, fl, fs);
    unsigned long cs = 0;
    for (unsigned long i = 0; i < fl; i++) cs += (unsigned long)mb[i];
    printf("%lf\n", (cs / (double)100));
    free(mb);
    fclose(fs);
}


// Funcao fornecida: le uma string da entrada padrao que pode estar entre aspas ou ser "NULO"
// Strings entre aspas: extrai o conteudo interno sem as aspas
// "NULO" ou "nulo": armazena string vazia em 'str'
// Outros valores: lidos normalmente como token
void ScanQuoteString(char *str) {
    char R;
    // Consome espacos em branco iniciais
    while ((R = getchar()) != EOF && isspace(R));
    if (R == 'N' || R == 'n') {
        // Consome os 3 caracteres restantes de "ULO"
        getchar(); getchar(); getchar();
        strcpy(str, "");
    } else if (R == '\"') {
        // Le o conteudo entre aspas; se vazio, armazena string vazia
        if (scanf("%[^\"]", str) != 1) strcpy(str, "");
        getchar(); // consome o fecha-aspas
    } else if (R != EOF) {
        // Primeiro caractere ja lido; le o restante do token
        str[0] = R;
        scanf("%s", &str[1]);
    } else {
        strcpy(str, "");
    }
}

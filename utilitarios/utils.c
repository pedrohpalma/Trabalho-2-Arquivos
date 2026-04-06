#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

// extrai o proximo campo do CSV avançando o ponteiro da linha. feito por conta de dificuldade em fazer strtok funcionar
void getProxCampo(char **line_ptr, char *field) {
    char *p = *line_ptr;
    int i = 0;
    // le ate nova linha ou virgula
    while (*p && *p != ',' && *p != '\n' && *p != '\r') {
        field[i++] = *p++;
    }
    field[i] = '\0';
    if (*p == ',') {
        p++; //pula o separador
    }
    *line_ptr = p;
}


void BinarioNaTela(char *arquivo) {
    FILE *fs;
    if (arquivo == NULL || !(fs = fopen(arquivo, "rb"))) {
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

void ScanQuoteString(char *str) {
    char R;
    while ((R = getchar()) != EOF && isspace(R));
    if (R == 'N' || R == 'n') {
        getchar(); getchar(); getchar();
        strcpy(str, "");
    } else if (R == '\"') {
        if (scanf("%[^\"]", str) != 1) strcpy(str, "");
        getchar();
    } else if (R != EOF) {
        str[0] = R;
        scanf("%s", &str[1]);
    } else {
        strcpy(str, "");
    }
}

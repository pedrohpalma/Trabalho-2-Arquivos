#ifndef UTILS_H
#define UTILS_H


// lê o arquivo binário, calcula a soma de todos os seus bytes e imprime o resultado dividido por 100
void BinarioNaTela(char *arquivo);

// Função para padronização de leitura de strings
void ScanQuoteString(char *str);

// Pega o próximo campo de uma linha CSV, atualizando o ponteiro para a posição do próximo campo
void getProxCampo(char **line_ptr, char *field);

#endif

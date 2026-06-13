#ifndef UTILS_H
#define UTILS_H


/*
 * Lê o arquivo binário indicado por 'arquivo', calcula a soma de todos
 * os seus bytes e imprime o resultado dividido por 100.0.
 * Usada para validação da saída binária nos casos de teste.
 */
void BinarioNaTela(char *arquivo);

/*
 * Lê uma string da entrada padrão que pode estar entre aspas duplas
 * ou ser a palavra "NULO". Se entre aspas, extrai apenas o conteúdo
 * interno. Se for "NULO" (ou "nulo"), armazena string vazia em 'str'.
 * Trata espaços antes da string e consome o fecha-aspas final.
 */
void ScanQuoteString(char *str);

/*
 * Extrai o próximo campo delimitado por vírgula (ou fim de linha) da
 * string apontada por '*line_ptr', copiando-o para 'field' e avançando
 * o ponteiro para além do delimitador.
 * Alternativa a strtok para evitar problemas com estado global.
 */
void getProxCampo(char **line_ptr, char *field);

#endif

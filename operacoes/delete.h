#ifndef DELETE_H
#define DELETE_H

/*
 * Funcionalidade 4 – DELETE FROM (remoção lógica com múltiplos critérios):
 * Executa 'n' remoções no arquivo binário 'arqBin'. Para cada remoção,
 * lê 'm' critérios e marca como removidos ('1') os registros ativos que
 * os satisfaçam, empilhando-os na lista de registros reutilizáveis.
 * Marca o arquivo como inconsistente durante a operação e restaura ao
 * final. Imprime o checksum via BinarioNaTela ao final.
 */
void func4(char *arqBin, int n);

#endif

#ifndef SEARCH_H
#define SEARCH_H

/*
 * Funcionalidade 3 – SELECT * FROM WHERE (busca com múltiplos critérios):
 * Executa 'n' buscas no arquivo binário 'arqBin'. Para cada busca, lê
 * 'm' pares (campo, valor) da entrada padrão e percorre o arquivo
 * imprimindo os registros ativos que satisfaçam todos os critérios.
 * Quando o campo de busca é 'codEstacao' (chave única), a varredura é
 * interrompida após o primeiro registro correspondente ser encontrado.
 * Imprime "Registro inexistente." se nenhum registro for encontrado.
 */
void func3(char *arqBin, int n);

#endif

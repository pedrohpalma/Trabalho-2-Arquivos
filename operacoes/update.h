#ifndef UPDATE_H
#define UPDATE_H

/*
 * Funcionalidade 6 – UPDATE (atualização de campos):
 * Executa 'n' atualizações no arquivo binário 'arqBin'. Para cada
 * atualização, lê 'm' critérios de busca e 'p' pares (campo, valor)
 * de atualização, aplicando as mudanças a todos os registros ativos
 * que satisfaçam os critérios. Reescreve cada registro modificado no
 * mesmo offset. Restaura a consistência do cabeçalho ao final.
 */
void func6(char *arqBin, int n);

#endif

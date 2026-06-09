#ifndef WRITE_H
#define WRITE_H

/*
 * Funcionalidade 1 – CREATE TABLE (CSV → Binário):
 * Lê o arquivo CSV 'arqEntrada', converte cada linha em um Registro e
 * grava no arquivo binário 'arqSaida'. Inicializa e atualiza o cabeçalho
 * com contagens de estações e pares únicos. Ao final imprime o checksum
 * do binário gerado via BinarioNaTela.
 */
void func1(char *arqEntrada, char *arqSaida);

/*
 * Funcionalidade 5 – INSERT INTO:
 * Insere 'n' novos registros no arquivo binário 'arqBin', lendo-os da
 * entrada padrão. Reutiliza espaços de registros logicamente removidos
 * (pilha gerenciada pelo campo 'topo' do cabeçalho) antes de inserir
 * ao final. Atualiza o cabeçalho e imprime o checksum ao final.
 */
void func5(char *arqBin, int n);

#endif

all:
	gcc -Wall -Wextra -std=c11 -g -o programaTrab main.c arquivos/arquivos.c cabecalho/cabecalho.c operacoes/operacoes.c registro/registro.c utilitarios/utils.c -lm

run:
	./programaTrab
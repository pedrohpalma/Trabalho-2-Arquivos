all:
	gcc -Wall -Wextra -std=c11 -g -o programaTrab main.c arquivos/arquivos.c cabecalho/cabecalho.c operacoes/write.c operacoes/read.c operacoes/search.c operacoes/delete.c operacoes/update.c registro/registro.c utilitarios/utils.c -lm

run:
	./programaTrab

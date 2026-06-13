all:
	gcc -Wall -Wextra -std=c11 -g -o programaTrab main.c arquivos/*.c cabecalho/*.c operacoes/*.c registro/*.c utilitarios/*.c cabecalhoArvoreB/*.c noArvoreB/*.c arvoreB/*.c -lm

run:
	./programaTrab

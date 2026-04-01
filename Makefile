CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

TARGET = trabalho1

SRC = \
	main.c \
	arquivos/arquivos.c \
	cabecalho/cabecalho.c \
	operacoes/operacoes.c \
	registro/registro.c \
	utilitarios/utils.c

OBJ = $(SRC:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ) $(TARGET)

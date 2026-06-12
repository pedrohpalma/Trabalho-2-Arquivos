#ifndef OPERACOES_H
#define OPERACOES_H

/*
 * Agrega as funcionalidades do programa, organizadas em módulos
 * por categoria funcional:
 *
 *   write.h  — func1 (CSV→Binário), func5 (INSERT)
 *   read.h   — func2 (SELECT *)
 *   search.h — func3 (SELECT WHERE)
 *   delete.h — func4 (DELETE)
 *   update.h — func6 (UPDATE)
 *   func7    — cria indice arvore-B
 *   func8    — SELECT WHERE usando indice arvore-B quando possivel
 *
 * Inclua este arquivo para ter acesso a todas as funcionalidades de uma vez.
 */

#include "write.h"
#include "read.h"
#include "search.h"
#include "delete.h"
#include "update.h"

void func7(char *arqEntrada, char *arqIndice);
void func8(char *arqEntrada, char *arqIndice, int n);

#endif

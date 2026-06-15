#ifndef OPERACOES_H
#define OPERACOES_H

/*
    Trab0:
    func1 - Create Table: le o .csv, converte para binario e escreve o cabecalho
    func2 - Select *: percorre o binario e imprime todos os registros nao removidos
    func3 - Select Where: executa 'n' buscas com multiplos criterios no arquivo binario
    func4 - Delete From: realiza 'n' remocoes logicas com multiplos criterios
    func5 - Insert Into: insere 'n' registros, reaproveitando espacos de removidos quando disponivel
    func6 - Update: atualiza campos de registros que satisfacam os criterios de busca

    Trab1:
    func7 - Create Index: cria um índice a partir do arquivo de dados binário
    func8 - Search:  busca sequencial quando a busca não inclui o campo codEstacao
    func9 - Insert: insere registro no arquivo de dados priorizando reaproveitamento de removidos ou colocando no final
    func10 - Delete: coordena a remocao de registros, apagando no arquivo de dados e removendo as chaves da arvoreB
*/


#include "func1.h"
#include "func2.h"
#include "func3.h"
#include "func4.h"
#include "func5.h"
#include "func6.h"
#include "func7.h"
#include "func8.h"
#include "func9.h"
#include "func10.h"

#endif

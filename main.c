#include <stdio.h>
#include <stdlib.h>

int main() {
    int funcionalidade;
    char arquivoCSV[100];
    char arquivoBin[100];
    int n; //qtd de buscas/remoções

    //le identificador de funcionalidade(1 a 6)
    if (scanf("%d", &funcionalidade) != 1) {
        return 0;
    }

    switch (funcionalidade) {
        case 1:
            // Sintaxe: 1 arquivoEntrada.csv arquivoSaida.bin
            scanf("%s %s", arquivoCSV, arquivoBin);
            //funcao1(arquivoEntrada, arquivoSaida)
            //binarionatela
            break;

        case 2:
            // Sintaxe: 2 arquivoEntrada.bin
            scanf("%s", arquivoBin);
            // funcionalidade2(arquivoEntrada);
            // binarionatela
            break;

        case 3:
            scanf("%s %d", arquivoBin, &n);
            //funcionalidade3(arquivoBin, n)
            break;

        case 4:
            // Sintaxe: 4 arquivoEntrada.bin n
            scanf("%s %d", arquivoBin, &n);
            // funcionalidade4(arquivoBin, n);
            break;

        case 5:
            // Sintaxe: 5 arquivoEntrada.bin n
            scanf("%s %d", arquivoBin, &n);
            // funcionalidade5(arquivoBin, n);
            break;

        case 6:
            // Sintaxe: 6 arquivoEntrada.bin n 
            scanf("%s %d", arquivoBin, &n);
            // funcionalidade6(arquivoBin, n);
            break;

        default:
            // Caso receba um número inválido
            break;
    }

    return 0;
}
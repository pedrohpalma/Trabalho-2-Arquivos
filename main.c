#include <stdio.h>
#include "operacoes/operacoes.h"

int main() {
    int operacao;
    
    // Lê a primeira entrada para definir a funcionalidade
    if (scanf("%d", &operacao) != 1) {
        return 0;
    }

    // Variáveis auxiliares para leitura dos parâmetros do terminal
    char arqEntrada[100];
    char arqSaida[100];
    char arqBin[100];
    int n;

    switch (operacao) {
        case 1:
            // Funcionalidade 1: CSV -> Binário
            scanf("%s %s", arqEntrada, arqSaida);
            funcionalidade_1(arqEntrada, arqSaida);
            break;

        case 2:
            // Funcionalidade 2: Select *
            scanf("%s", arqBin);
            funcionalidade_2(arqBin);
            break;

        case 3:
            // Funcionalidade 3: Select com múltiplos critérios
            scanf("%s %d", arqBin, &n);
            funcionalidade_3(arqBin, n);
            break;

        case 4:
            // Funcionalidade 4: Remoção lógica com múltiplos critérios
            scanf("%s %d", arqBin, &n);
            funcionalidade_4(arqBin, n);
            break;

        case 5:
            // Funcionalidade 5: Insert
            scanf("%s %d", arqBin, &n);
            funcionalidade_5(arqBin, n);
            break;

        case 6:
            // Funcionalidade 6: Update in-place
            scanf("%s %d", arqBin, &n);
            funcionalidade_6(arqBin, n);
            break;

        default:
            printf("Funcionalidade invalida.\n");
            break;
    }

    return 0;
}
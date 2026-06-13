// Pedro Hamamoto da Palma - 16818280
// Mateus Juares Felipe - 16891602
// Turma de segunda feira

#include <stdio.h>
#include "operacoes/operacoes.h"

int main()
{
    int operacao;

    // Lê a primeira entrada para definir a funcionalidade
    if (scanf("%d", &operacao) != 1)
    {
        return 0;
    }

    // Variáveis auxiliares para leitura dos parâmetros do terminal
    char arqEntrada[100];
    char arqSaida[100];
    char arqBin[100];
    int n;

    switch (operacao)
    {
    case 1:
        // Funcionalidade 1: CSV -> Binário
        scanf("%s %s", arqEntrada, arqSaida);
        func1(arqEntrada, arqSaida);
        break;

    case 2:
        // Funcionalidade 2: Select *
        scanf("%s", arqBin);
        func2(arqBin);
        break;

    case 3:
        // Funcionalidade 3: Select com múltiplos critérios
        scanf("%s %d", arqBin, &n);
        func3(arqBin, n);
        break;

    case 4:
        // Funcionalidade 4: Remoção lógica com múltiplos critérios
        scanf("%s %d", arqBin, &n);
        func4(arqBin, n);
        break;

    case 5:
        // Funcionalidade 5: Insert
        scanf("%s %d", arqBin, &n);
        func5(arqBin, n);
        break;

    case 6:
        // Funcionalidade 6: Update
        scanf("%s %d", arqBin, &n);
        func6(arqBin, n);
        break;

    case 7:
        // Funcionalidade 7: Criacao de indice arvore-B
        scanf("%s %s", arqEntrada, arqSaida);
        func7(arqEntrada, arqSaida);
        break;

    case 8:
        // Funcionalidade 8: Select com indice arvore-B
        scanf("%s %s %d", arqEntrada, arqSaida, &n);
        func8(arqEntrada, arqSaida, n);
        break;

    case 9:
        // Funcionalidade 9: Insert atualizando indice arvore-B
        scanf("%s %s %d", arqEntrada, arqSaida, &n);
        func9(arqEntrada, arqSaida, n);
        break;

    case 10:
        // Funcionalidade 10: Remocao atualizando indice arvore-B
        scanf("%s %s %d", arqEntrada, arqSaida, &n);
        func10(arqEntrada, arqSaida, n);
        break;

    default:
        printf("Funcionalidade invalida.\n");
        break;
    }

    return 0;
}

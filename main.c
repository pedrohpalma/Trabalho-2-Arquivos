#include <stdio.h>
#include "operacoes/operacoes.h"

int main() {
    int operacao;
    
    // Lê a primeira entrada para definir a funcionalidade
    if (scanf("%d", &operacao) != 1) {
        return 0;
    }

    if (operacao == 1) {
        char arqEntrada[100];
        char arqSaida[100];
        
        // Lê os nomes dos arquivos
        scanf("%s %s", arqEntrada, arqSaida);
        funcionalidade_1(arqEntrada, arqSaida);
    } 
    // Outras operações poderiam ser implementadas na sequência
    return 0;
}
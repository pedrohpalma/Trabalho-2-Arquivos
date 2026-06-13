#ifndef REGISTRO_H
#define REGISTRO_H
#include <stdio.h>

#define TAM_REGISTRO 80

/*
 * Representa um registro de estação de metrô com todos os seus campos.
 * Campos de tamanho variável (nomeEstacao, nomeLinha) são precedidos
 * por seus respectivos tamanhos. O campo 'removido' controla a remoção
 * lógica ('0' = ativo, '1' = removido). O campo 'proximo' é usado como
 * ponteiro da pilha de registros logicamente removidos (-1 = sem próximo).
 * Campos inteiros com valor -1 indicam ausência de dado (NULO).
 */
typedef struct{
    char removido;
    int proximo;
    int codEstacao;
    int codLinha;
    int codProxEstacao;
    int distProxEstacao;
    int codLinhaIntegra;
    int codEstIntegra;
    int tamNomeEstacao;
    char nomeEstacao[100];
    int tamNomeLinha;
    char nomeLinha[100];
} Registro;

/*
 * Estrutura para representar um critério de busca, contendo o nome do campo,
 * o valor esperado (tanto em string quanto em inteiro, dependendo do tipo),
 * e um indicador se a busca é por campo nulo.
 */
typedef struct {
    char nomeCampo[50];
    int valorInt;
    char valorStr[100];
    int isNulo;
} CampoBusca;

/*
 * Lê uma linha do CSV e preenche a struct Registro.
 * Campos ausentes (vazios) são armazenados como -1 (inteiros) ou
 * string vazia com tamanho 0 (strings). Define removido='0' e proximo=-1.
 */
void lerRegistroCSV(char *linha, Registro *r);

/*
 * Serializa e escreve o Registro no arquivo binário apontado por 'bin'.
 * Garante exatamente TAM_REGISTRO (80) bytes por registro, preenchendo
 * o espaço restante com o caractere '$'.
 */
void escreveRegistroBin(FILE *bin, Registro *r);

/*
 * Lê exatamente TAM_REGISTRO bytes do arquivo binário e desserializa
 * para a struct Registro.
 * Retorna: 0 = EOF (fim do arquivo),
 *          1 = registro lido com sucesso,
 *          2 = registro logicamente removido (pulado).
 */
int leRegistroBin(FILE *bin, Registro *r);

/*
 * Imprime os campos do Registro na saída padrão, exibindo "NULO"
 * para campos ausentes (inteiro == -1 ou string com tamanho 0).
 */
void imprimeRegistro(Registro *r);

/*
 * Verifica se o Registro satisfaz um critério de busca.
 * 'campo' é o nome do campo a comparar, 'valorStr' e 'valorInt' são os
 * valores esperados (apenas o adequado ao tipo do campo é usado),
 * 'isNulo' indica se a busca é por campo nulo.
 * Retorna 1 se o critério é atendido, 0 caso contrário.
 */
int atendeCriterio(Registro *r, char *campo, char *valorStr, int valorInt, int isNulo);

/*
 * Lê os campos de um novo Registro a partir da entrada padrão (teclado),
 * no formato esperado pelo programa. Strings entre aspas são tratadas
 * pela função ScanQuoteString. Campos "NULO" são convertidos para -1
 * (inteiros) ou string vazia (strings). Define removido='0' e proximo=-1.
 */
void leRegistroTeclado(Registro *r);

/*
 * Atualiza um campo específico do Registro com o novo valor fornecido.
 * 'campo' indica qual campo alterar; 'valorStr', 'valorInt' e 'isNulo'
 * fornecem o novo valor de acordo com o tipo do campo.
 */
void atualizaCampo(Registro *r, char *campo, char *valorStr, int valorInt, int isNulo);

/*
 * Verifica se um campo é do tipo string.
 * Retorna 1 se for string, 0 caso contrário.
 */
int campoString(char *campo);

/*
 * Lê os critérios de busca a partir da entrada padrão, preenchendo o array de CampoBusca.
 * 'qtdCriterios' indica quantos critérios serão lidos.
 * 'possuiCodEstacao' é setado para 1 se algum dos critérios
 * for por 'codEstacao' (campo chave único), o que pode otimizar buscas futuras.
 * 'valorCodEstacao' é preenchido com o valor do critério 'cod
 */
void lerCriteriosBusca(CampoBusca criterios[], int qtdCriterios, int *possuiCodEstacao, int *valorCodEstacao);

/*
 * Verifica se o Registro satisfaz todos os critérios de busca fornecidos
 * Retorna 1 se o registro atende a todos os critérios, 0 caso contrário.
 * Registros removidos são automaticamente considerados como não satisfazendo os critérios.
 */
int registroSatisfazCriterios(Registro *r, CampoBusca criterios[], int qtdCriterios);

#endif

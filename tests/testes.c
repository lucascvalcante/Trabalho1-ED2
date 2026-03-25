#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "exhash.h"

int testes_passaram = 0;
int testes_falharam = 0;

#define TESTE(condicao, nome_teste) \
    do { \
        if (condicao) { \
            printf("[  OK  ] %s\n", nome_teste); \
            testes_passaram++; \
        } else { \
            printf("[ ERRO ] %s\n", nome_teste); \
            testes_falharam++; \
        } \
    } while(0)


typedef struct {
    int valor;
} Dado;


void limpar_arquivos_teste() {
    remove("teste_hash.bin");
    remove("teste_hash.bin_dir.bin");
}

int main() {
    printf("=======================================\n");
    printf("  BATERIA DE TESTES - HASH EXTENSIVEL  \n");
    printf("=======================================\n\n");

    limpar_arquivos_teste();
    exHash hash = init_exHash("teste_hash.bin", 2, sizeof(Dado));
    TESTE(hash != NULL, "Inicializar Hash do zero");

    Dado d1 = {100};
    Dado d2 = {200};
    Dado d3 = {300}; 

    TESTE(insert_exHash(hash, 10, &d1) == true, "Inserir primeiro elemento (Chave 10)");
    TESTE(insert_exHash(hash, 20, &d2) == true, "Inserir segundo elemento (Chave 20)");
    TESTE(insert_exHash(hash, 10, &d1) == false, "Barrar insercao de chave duplicada (Chave 10)");
    TESTE(insert_exHash(hash, 30, &d3) == true, "Inserir terceiro elemento e forcar SPLIT (Chave 30)");

    Dado* busca_sucesso = (Dado*) search_exHash(hash, 20);
    TESTE(busca_sucesso != NULL && busca_sucesso->valor == 200, "Buscar elemento existente (Chave 20)");
    if (busca_sucesso) free(busca_sucesso);

    Dado* busca_falha = (Dado*) search_exHash(hash, 99);
    TESTE(busca_falha == NULL, "Buscar elemento inexistente (Chave 99)");

    TESTE(remove_exHash(hash, 10) == true, "Remover elemento existente (Chave 10)");
    TESTE(remove_exHash(hash, 10) == false, "Tentar remover elemento ja removido (Chave 10)");
    TESTE(search_exHash(hash, 10) == NULL, "Garantir que busca falha apos remocao (Chave 10)");

    close_exHash(hash);

    exHash hash_carregado = load_exHash("teste_hash.bin");
    TESTE(hash_carregado != NULL, "Carregar Hash existente do disco");
    Dado* busca_sobrevivente = (Dado*) search_exHash(hash_carregado, 30);
    TESTE(busca_sobrevivente != NULL && busca_sobrevivente->valor == 300, "Recuperar dado apos reload do arquivo");
    if (busca_sobrevivente) free(busca_sobrevivente);

    close_exHash(hash_carregado);
    limpar_arquivos_teste();

    printf("\n=======================================\n");
    printf("RESULTADO FINAL:\n");
    printf("Passaram: %d\n", testes_passaram);
    printf("Falharam: %d\n", testes_falharam);
    printf("=======================================\n");

    return (testes_falharam == 0) ? 0 : 1;
}
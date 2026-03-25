#include "unity.h"
#include "exhash.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// --- ESTRUTURA FAKE PARA OS TESTES ---
typedef struct {
    int valor;
} Dado;

// Variável global para ser usada nos testes
exHash hash = NULL; 

// --- SETUP E TEARDOWN EXIGIDOS PELO UNITY ---
// Roda ANTES de cada teste
void setUp(void) {
    remove("teste_hash.bin");
    remove("teste_hash.bin_dir.bin");
    // Já cria o hash fresco para cada teste usar
    hash = init_exHash("teste_hash.bin", 2, sizeof(Dado)); 
}

// Roda DEPOIS de cada teste
void tearDown(void) {
    if (hash != NULL) {
        close_exHash(hash);
        hash = NULL;
    }
    remove("teste_hash.bin");
    remove("teste_hash.bin_dir.bin");
}

// --- CASOS DE TESTE ---

void test_InicializarHashDoZero_DeveRetornarPonteiroValido(void) {
    TEST_ASSERT_NOT_NULL(hash);
}

void test_InserirElementos_E_ForcarSplit_DeveRetornarTrue(void) {
    Dado d1 = {100};
    Dado d2 = {200};
    Dado d3 = {300}; // Força o split (tamanho do bucket é 2)

    TEST_ASSERT_TRUE(insert_exHash(hash, 10, &d1));
    TEST_ASSERT_TRUE(insert_exHash(hash, 20, &d2));
    TEST_ASSERT_TRUE(insert_exHash(hash, 30, &d3));
}

void test_InserirChaveDuplicada_DeveRetornarFalse(void) {
    Dado d1 = {100};
    TEST_ASSERT_TRUE(insert_exHash(hash, 10, &d1));
    // Tenta de novo com a mesma chave
    TEST_ASSERT_FALSE(insert_exHash(hash, 10, &d1)); 
}

void test_BuscarElementoExistente_DeveRetornarDadoCorreto(void) {
    Dado d2 = {200};
    insert_exHash(hash, 20, &d2);

    Dado* busca = (Dado*) search_exHash(hash, 20);
    
    TEST_ASSERT_NOT_NULL(busca);
    TEST_ASSERT_EQUAL_INT(200, busca->valor);
    
    free(busca);
}

void test_BuscarElementoInexistente_DeveRetornarNull(void) {
    // "Caminho Infeliz" pedido pelo professor
    Dado* busca = (Dado*) search_exHash(hash, 99);
    TEST_ASSERT_NULL(busca);
}

void test_RemoverElementoExistente_DeveRetornarTrue(void) {
    Dado d1 = {100};
    insert_exHash(hash, 10, &d1);

    TEST_ASSERT_TRUE(remove_exHash(hash, 10));
    
    // Garante que sumiu mesmo
    Dado* busca = (Dado*) search_exHash(hash, 10);
    TEST_ASSERT_NULL(busca);
}

// --- MAIN DO UNITY ---
int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_InicializarHashDoZero_DeveRetornarPonteiroValido);
    RUN_TEST(test_InserirElementos_E_ForcarSplit_DeveRetornarTrue);
    RUN_TEST(test_InserirChaveDuplicada_DeveRetornarFalse);
    RUN_TEST(test_BuscarElementoExistente_DeveRetornarDadoCorreto);
    RUN_TEST(test_BuscarElementoInexistente_DeveRetornarNull);
    RUN_TEST(test_RemoverElementoExistente_DeveRetornarTrue);
    
    return UNITY_END();
}
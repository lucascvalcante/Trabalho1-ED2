#include "../unity.h"
#include "../exHash.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int valor;
} Dado;

// Callback auxiliar para o teste do foreach
static void callback_teste_conta(const char* chave, void* dado, void* extra) {
    int* contador = (int*)extra;
    (*contador)++;
}

exHash hash = NULL; 

void setUp(void) {

    remove("teste_hash.hf");
    remove("teste_hash.dir");
    hash = init_exHash("teste_hash.hf", 2, sizeof(Dado)); 
}

void tearDown(void) {
    if (hash != NULL) {
        close_exHash(hash);
        hash = NULL;
    }

    remove("teste_hash.hf");
    remove("teste_hash.dir");
    remove("teste_hash_dump.hfd");
}

void test_InitHash(void) {
    TEST_ASSERT_NOT_NULL(hash);
}

void test_InsertElement(void) {
    Dado d1 = {100};
    Dado d2 = {200};
    Dado d3 = {300}; 

    TEST_ASSERT_TRUE(insert_exHash(hash, "CEP10", &d1));
    TEST_ASSERT_TRUE(insert_exHash(hash, "CEP20", &d2));
    TEST_ASSERT_TRUE(insert_exHash(hash, "CEP30", &d3));
}

void test_InsertDuplicatedKey(void) {
    Dado d1 = {100};
    TEST_ASSERT_TRUE(insert_exHash(hash, "CEP10", &d1));
    TEST_ASSERT_FALSE(insert_exHash(hash, "CEP10", &d1)); 
}

void test_SearchExistingElements(void) {
    Dado d2 = {200};
    insert_exHash(hash, "CEP20", &d2);

    Dado* busca = (Dado*) search_exHash(hash, "CEP20");
    
    TEST_ASSERT_NOT_NULL(busca);
    TEST_ASSERT_EQUAL_INT(200, busca->valor);
    
    free(busca); 
}

void test_SearchUnexistingElements(void) {
    Dado* busca = (Dado*) search_exHash(hash, "NAO_EXISTE");
    TEST_ASSERT_NULL(busca);
}

void test_RemoveExistingElement(void) {
    Dado d1 = {100};
    insert_exHash(hash, "CEP10", &d1);

    TEST_ASSERT_TRUE(remove_exHash(hash, "CEP10"));
    Dado* busca = (Dado*) search_exHash(hash, "CEP10");
    TEST_ASSERT_NULL(busca);
}

void test_LoadHash(void) {
    Dado d1 = {999};
    insert_exHash(hash, "PERSISTE", &d1);
    close_exHash(hash);
    hash = NULL;
    hash = load_exHash("teste_hash.hf");
    TEST_ASSERT_NOT_NULL(hash);
    
    Dado* busca = (Dado*) search_exHash(hash, "PERSISTE");
    TEST_ASSERT_NOT_NULL(busca);
    TEST_ASSERT_EQUAL_INT(999, busca->valor);
    
    free(busca);
}

void test_UpdateElement(void) {
    Dado d1 = {100};
    insert_exHash(hash, "CEP10", &d1);

    Dado d_novo = {500};
    TEST_ASSERT_TRUE(update_exHash(hash, "CEP10", &d_novo));

    Dado* busca = (Dado*) search_exHash(hash, "CEP10");
    TEST_ASSERT_NOT_NULL(busca);
    TEST_ASSERT_EQUAL_INT(500, busca->valor);
    free(busca);

    Dado d_fake = {999};
    TEST_ASSERT_FALSE(update_exHash(hash, "NAO_EXISTE", &d_fake));
}

void test_ForeachElement(void) {
    Dado d1 = {10};
    insert_exHash(hash, "C1", &d1);
    insert_exHash(hash, "C2", &d1);
    insert_exHash(hash, "C3", &d1);

    int contador = 0;
    foreach_exHash(hash, callback_teste_conta, &contador);    
    TEST_ASSERT_EQUAL_INT(3, contador);
    remove_exHash(hash, "C2");
    
    contador = 0; 
    foreach_exHash(hash, callback_teste_conta, &contador);    
    TEST_ASSERT_EQUAL_INT(2, contador);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_InitHash);
    RUN_TEST(test_InsertElement);
    RUN_TEST(test_InsertDuplicatedKey);
    RUN_TEST(test_SearchExistingElements);
    RUN_TEST(test_SearchUnexistingElements);
    RUN_TEST(test_RemoveExistingElement);
    RUN_TEST(test_LoadHash);     
    RUN_TEST(test_UpdateElement);
    RUN_TEST(test_ForeachElement);
    
    return UNITY_END();
}
#include "unity.h"
#include "exhash.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int valor;
} Dado;

exHash hash = NULL; 

void setUp(void) {
    remove("teste_hash.bin");
    remove("teste_hash.bin_dir.bin");
    hash = init_exHash("teste_hash.bin", 2, sizeof(Dado)); 
}

void tearDown(void) {
    if (hash != NULL) {
        close_exHash(hash);
        hash = NULL;
    }
    remove("teste_hash.bin");
    remove("teste_hash.bin_dir.bin");
}


void test_InitHash(void) {
    TEST_ASSERT_NOT_NULL(hash);
}

void test_InsertElement(void) {
    Dado d1 = {100};
    Dado d2 = {200};
    Dado d3 = {300}; 

    TEST_ASSERT_TRUE(insert_exHash(hash, 10, &d1));
    TEST_ASSERT_TRUE(insert_exHash(hash, 20, &d2));
    TEST_ASSERT_TRUE(insert_exHash(hash, 30, &d3));
}

void test_InsertKey(void) {
    Dado d1 = {100};
    TEST_ASSERT_TRUE(insert_exHash(hash, 10, &d1));
    TEST_ASSERT_FALSE(insert_exHash(hash, 10, &d1)); 
}

void test_SearchExistingElements(void) {
    Dado d2 = {200};
    insert_exHash(hash, 20, &d2);

    Dado* busca = (Dado*) search_exHash(hash, 20);
    
    TEST_ASSERT_NOT_NULL(busca);
    TEST_ASSERT_EQUAL_INT(200, busca->valor);
    
    free(busca);
}

void test_SearchUnexistingElements(void) {
    Dado* busca = (Dado*) search_exHash(hash, 99);
    TEST_ASSERT_NULL(busca);
}

void test_RemoveExistingElement(void) {
    Dado d1 = {100};
    insert_exHash(hash, 10, &d1);

    TEST_ASSERT_TRUE(remove_exHash(hash, 10));
    Dado* busca = (Dado*) search_exHash(hash, 10);
    TEST_ASSERT_NULL(busca);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_InitHash);
    RUN_TEST(test_InsertElement);
    RUN_TEST(test_InsertKey);
    RUN_TEST(test_SearchExistingElements);
    RUN_TEST(test_SearchUnexistingElements);
    RUN_TEST(test_RemoveExistingElement);
    
    return UNITY_END();
}
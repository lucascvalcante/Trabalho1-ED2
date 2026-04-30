#include "../unity.h"
#include "../pm.h"
#include "../habitante.h"
#include "../exHash.h"
#include <stdio.h>
#include <stdlib.h>

exHash hash_teste = NULL;
const char* ARQUIVO_MOCK = "teste_mock.pm";

void setUp(void) {
    remove("db_pm_teste.hf");
    remove("db_pm_teste.dir");
    hash_teste = init_exHash("db_pm_teste.hf", 2, get_habitante_size());

    FILE *f = fopen(ARQUIVO_MOCK, "w");
    if (f != NULL) {
        fprintf(f, "p 123.456 Ada Lovelace F 10/12/1815\n");
        fprintf(f, "p 999.999 Alan Turing M 23/06/1912\n");
        fprintf(f, "m 123.456 86000-000 N 42.5 Apto 1 Bloco B\n");
        fclose(f);
    }
}

void tearDown(void) {
    if (hash_teste != NULL) close_exHash(hash_teste);
    
    remove("db_pm_teste.hf");
    remove("db_pm_teste.dir");
    remove(ARQUIVO_MOCK);
}

void test_processa_pm_leitura_e_vinculo(void) {
    processa_arquivo_pm(ARQUIVO_MOCK, hash_teste);

    Habitante ada = (Habitante) search_exHash(hash_teste, "123.456");
    TEST_ASSERT_NOT_NULL(ada);
    TEST_ASSERT_EQUAL_STRING("Ada", get_habitante_nome(ada));
    TEST_ASSERT_EQUAL_STRING("Lovelace", get_habitante_sobrenome(ada));
    TEST_ASSERT_EQUAL_CHAR('F', get_habitante_sexo(ada));
    
    TEST_ASSERT_FALSE(is_sem_teto(ada));
    TEST_ASSERT_EQUAL_STRING("86000-000", get_habitante_cep(ada));
    TEST_ASSERT_EQUAL_CHAR('N', get_habitante_face(ada));
    TEST_ASSERT_EQUAL_DOUBLE(42.5, get_habitante_numero(ada));
    TEST_ASSERT_EQUAL_STRING("Apto 1 Bloco B", get_habitante_complemento(ada));

    Habitante alan = (Habitante) search_exHash(hash_teste, "999.999");
    TEST_ASSERT_NOT_NULL(alan);
    TEST_ASSERT_EQUAL_STRING("Alan", get_habitante_nome(alan));
    TEST_ASSERT_TRUE(is_sem_teto(alan));

    free_habitante(ada);
    free_habitante(alan);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_processa_pm_leitura_e_vinculo);
    
    return UNITY_END();
}
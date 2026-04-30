#include "../unity.h"
#include "../exHash.h"
#include "../geo.h"
#include "../quadra.h"
#include <stdio.h>
#include <stdlib.h>


exHash banco_quadras = NULL;
const char* nome_arquivo_geo = "teste_mock.geo";
const char* nome_arquivo_hf = "banco_quadras_test.hf";

void setUp(void) {
    FILE* f_geo = fopen(nome_arquivo_geo, "w");
    if (f_geo) {
        fprintf(f_geo, "q 11.11 10.0 20.0 100.0 150.0\n");
        fprintf(f_geo, "cq 2.5px red blue\n");
        fprintf(f_geo, "q 22.22 300.0 400.0 50.0 50.0\n");
        fclose(f_geo);
    }

    banco_quadras = init_exHash(nome_arquivo_hf, 4, get_tamanho_quadra());
    processa_arquivo_geo(nome_arquivo_geo, banco_quadras);
}

void tearDown(void) {
    if (banco_quadras) {
        close_exHash(banco_quadras);
        banco_quadras = NULL;
    }
    remove(nome_arquivo_geo);
    remove(nome_arquivo_hf);
    remove("banco_quadras_test.hfc"); 
}

void test_leitura_quadra_padrao(void) {
    Quadra q1 = search_exHash(banco_quadras, "11.11");
    TEST_ASSERT_NOT_NULL_MESSAGE(q1, "A quadra 11.11 deveria ter sido encontrada no Hash.");

    TEST_ASSERT_EQUAL_STRING("11.11", get_quadra_cep(q1));
    TEST_ASSERT_EQUAL_DOUBLE(10.0, get_quadra_x(q1));
    TEST_ASSERT_EQUAL_DOUBLE(20.0, get_quadra_y(q1));
    TEST_ASSERT_EQUAL_DOUBLE(100.0, get_quadra_w(q1));
    TEST_ASSERT_EQUAL_DOUBLE(150.0, get_quadra_h(q1));

    TEST_ASSERT_EQUAL_STRING("1.0px", get_quadra_sw(q1));
    TEST_ASSERT_EQUAL_STRING("white", get_quadra_cfill(q1));
    TEST_ASSERT_EQUAL_STRING("black", get_quadra_cstrk(q1));

    free_quadra(q1);
}

void test_leitura_quadra_com_cq(void) {
    Quadra q2 = search_exHash(banco_quadras, "22.22");
    TEST_ASSERT_NOT_NULL_MESSAGE(q2, "A quadra 22.22 deveria ter sido encontrada no Hash.");
    TEST_ASSERT_EQUAL_STRING("22.22", get_quadra_cep(q2));
    TEST_ASSERT_EQUAL_DOUBLE(300.0, get_quadra_x(q2));
    TEST_ASSERT_EQUAL_DOUBLE(400.0, get_quadra_y(q2));
    TEST_ASSERT_EQUAL_DOUBLE(50.0, get_quadra_w(q2));
    TEST_ASSERT_EQUAL_DOUBLE(50.0, get_quadra_h(q2));
    TEST_ASSERT_EQUAL_STRING("2.5px", get_quadra_sw(q2));
    TEST_ASSERT_EQUAL_STRING("red", get_quadra_cfill(q2));
    TEST_ASSERT_EQUAL_STRING("blue", get_quadra_cstrk(q2));

    free_quadra(q2);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_leitura_quadra_padrao);
    RUN_TEST(test_leitura_quadra_com_cq);
    
    return UNITY_END();
}
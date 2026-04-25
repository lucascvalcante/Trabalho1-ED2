#include "../unity.h" 
#include "../quadra.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_criacao_quadra_e_getters(void) {
    Quadra q = cria_quadra("CEP123", 10.0, 20.0, 100.0, 50.0, "1.0", "blue", "red");
    TEST_ASSERT_NOT_NULL(q);
    TEST_ASSERT_EQUAL_STRING("CEP123", get_quadra_cep(q));
    TEST_ASSERT_EQUAL_DOUBLE(10.0, get_quadra_x(q));
    TEST_ASSERT_EQUAL_DOUBLE(20.0, get_quadra_y(q));
    TEST_ASSERT_EQUAL_DOUBLE(100.0, get_quadra_w(q));
    TEST_ASSERT_EQUAL_DOUBLE(50.0, get_quadra_h(q));
    TEST_ASSERT_EQUAL_STRING("1.0", get_quadra_sw(q));
    TEST_ASSERT_EQUAL_STRING("blue", get_quadra_cfill(q));
    TEST_ASSERT_EQUAL_STRING("red", get_quadra_cstrk(q));

    free_quadra(q);
}

void test_calculo_ancora_sudeste(void) {
    Quadra q = cria_quadra("CEP_TESTE", 10.0, 10.0, 50.0, 30.0, "1", "white", "black");
    
    double ax, ay;
    get_quadra_ancora(q, &ax, &ay);
    TEST_ASSERT_EQUAL_DOUBLE(60.0, ax);
    TEST_ASSERT_EQUAL_DOUBLE(40.0, ay);

    free_quadra(q);
}

void test_seguranca_ponteiro_nulo(void) {
    TEST_ASSERT_NULL(get_quadra_cep(NULL));
    TEST_ASSERT_EQUAL_DOUBLE(0.0, get_quadra_x(NULL));
    TEST_ASSERT_NULL(get_quadra_cfill(NULL));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_criacao_quadra_e_getters);
    RUN_TEST(test_calculo_ancora_sudeste);
    RUN_TEST(test_seguranca_ponteiro_nulo);
    return UNITY_END();
}
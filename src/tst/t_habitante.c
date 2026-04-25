#include "../unity.h"
#include "../habitante.h"
#include <stdlib.h>
#include <string.h>

Habitante hab = NULL;

void setUp(void) {
    hab = cria_habitante("111.222.333-44", "Alan", "Turing", 'M', "23/06/1912");
}

void tearDown(void) {
    free_habitante(hab);
    hab = NULL;
}

void test_criacao_dados_pessoais(void) {
    TEST_ASSERT_NOT_NULL(hab);
    TEST_ASSERT_EQUAL_STRING("111.222.333-44", get_habitante_cpf(hab));
    TEST_ASSERT_EQUAL_STRING("Alan", get_habitante_nome(hab));
    TEST_ASSERT_EQUAL_STRING("Turing", get_habitante_sobrenome(hab));
    TEST_ASSERT_EQUAL_CHAR('M', get_habitante_sexo(hab));
    TEST_ASSERT_EQUAL_STRING("23/06/1912", get_habitante_nascimento(hab));
    TEST_ASSERT_TRUE(is_sem_teto(hab)); 
}

void test_controle_de_moradia(void) {
    set_habitante_endereco(hab, "86000-000", 'N', 150.5, "Apto 42");
    
    TEST_ASSERT_FALSE(is_sem_teto(hab));
    TEST_ASSERT_EQUAL_STRING("86000-000", get_habitante_cep(hab));
    TEST_ASSERT_EQUAL_CHAR('N', get_habitante_face(hab));
    TEST_ASSERT_EQUAL_DOUBLE(150.5, get_habitante_numero(hab));
    TEST_ASSERT_EQUAL_STRING("Apto 42", get_habitante_complemento(hab));

    set_habitante_sem_teto(hab);
    TEST_ASSERT_TRUE(is_sem_teto(hab));
    TEST_ASSERT_NULL(get_habitante_cep(hab)); 
}


void test_protecao_buffer_overflow_strings_longas(void) {
    const char* nome_gigante = "UmNomeExtremamenteLongoParaTestarSeOTratamentoDeBufferOverflowFunciona";
    Habitante h_longo = cria_habitante("123", nome_gigante, "Silva", 'M', "01/01");
    
    TEST_ASSERT_NOT_NULL(h_longo);
    TEST_ASSERT_EQUAL_INT(49, strlen(get_habitante_nome(h_longo)));
    
    free_habitante(h_longo);
}

void test_criacao_com_parametros_nulos(void) {
    Habitante h_nulo = cria_habitante(NULL, NULL, NULL, 'F', NULL);
    
    TEST_ASSERT_NOT_NULL(h_nulo);
    TEST_ASSERT_EQUAL_STRING("", get_habitante_nome(h_nulo));
    TEST_ASSERT_EQUAL_STRING("", get_habitante_cpf(h_nulo));
    
    free_habitante(h_nulo);
}

void test_seguranca_getters_com_habitante_nulo(void) {
    TEST_ASSERT_NULL(get_habitante_nome(NULL));
    TEST_ASSERT_NULL(get_habitante_cpf(NULL));
    TEST_ASSERT_EQUAL_CHAR('\0', get_habitante_sexo(NULL));
    TEST_ASSERT_DOUBLE_WITHIN(0.01, 0.0, get_habitante_numero(NULL));
    TEST_ASSERT_TRUE(is_sem_teto(NULL));
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_criacao_dados_pessoais);
    RUN_TEST(test_controle_de_moradia);
    RUN_TEST(test_protecao_buffer_overflow_strings_longas);
    RUN_TEST(test_criacao_com_parametros_nulos);
    RUN_TEST(test_seguranca_getters_com_habitante_nulo);
    
    return UNITY_END();
}
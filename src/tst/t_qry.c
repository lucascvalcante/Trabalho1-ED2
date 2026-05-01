#include "../unity.h"
#include "../qry.h"
#include "../exHash.h"
#include "../habitante.h"
#include "../quadra.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

exHash h_hab = NULL;
exHash h_quad = NULL;


void setUp(void) {
    h_hab = init_exHash("teste_hab.hf", 2, 256); 
    h_quad = init_exHash("teste_quad.hf", 2, 256); 
    
    remove("teste.qry");
    remove("teste_saida.txt");
    remove("teste_saida.svg");
}

void tearDown(void) {
    if (h_hab) close_exHash(h_hab);
    if (h_quad) close_exHash(h_quad);
    
    remove("teste_hab.hf");
    remove("teste_hab.hf.dir");
    remove("teste_quad.hf");
    remove("teste_quad.hf.dir");
    remove("teste.qry");
    remove("teste_saida.txt");
    remove("teste_saida.svg");
}


void test_ComandoNasc(void) {
    FILE* qry_file = fopen("teste.qry", "w");
    fprintf(qry_file, "nasc 111.111 Joao Silva M 01/01/2000\n");
    fclose(qry_file);

    processa_arquivo_qry("teste.qry", "teste_saida.txt", "teste_saida.svg", h_hab, h_quad);

    Habitante h = (Habitante) search_exHash(h_hab, "111.111");
    TEST_ASSERT_NOT_NULL(h);
    TEST_ASSERT_EQUAL_STRING("Joao", get_habitante_nome(h));
    TEST_ASSERT_EQUAL_STRING("Silva", get_habitante_sobrenome(h));
    TEST_ASSERT_TRUE(is_sem_teto(h));
}

void test_ComandoRip(void) {
    Habitante h_in = cria_habitante("222.222", "Morto", "Souza", 'M', "10/10/1950");
    insert_exHash(h_hab, "222.222", h_in);

    FILE* qry_file = fopen("teste.qry", "w");
    fprintf(qry_file, "rip 222.222\n");
    fclose(qry_file);

    processa_arquivo_qry("teste.qry", "teste_saida.txt", "teste_saida.svg", h_hab, h_quad);

    Habitante h_out = (Habitante) search_exHash(h_hab, "222.222");
    TEST_ASSERT_NULL(h_out);
}

void test_ComandoMud(void) {
    Habitante h_in = cria_habitante("333.333", "Maria", "Moura", 'F', "05/05/1990");
    insert_exHash(h_hab, "333.333", h_in);

    Quadra q_in = cria_quadra("CEP-123", 10.0, 10.0, 50.0, 50.0, "1.0px", "blue", "black");
    insert_exHash(h_quad, "CEP-123", q_in);

    FILE* qry_file = fopen("teste.qry", "w");
    fprintf(qry_file, "mud 333.333 CEP-123 N 10 apto-1\n");
    fclose(qry_file);

    processa_arquivo_qry("teste.qry", "teste_saida.txt", "teste_saida.svg", h_hab, h_quad);

    Habitante h_out = (Habitante) search_exHash(h_hab, "333.333");
    TEST_ASSERT_NOT_NULL(h_out);
    TEST_ASSERT_FALSE(is_sem_teto(h_out));
    TEST_ASSERT_EQUAL_STRING("CEP-123", get_habitante_cep(h_out));
    TEST_ASSERT_EQUAL_INT('N', get_habitante_face(h_out));
}

void test_ComandoDspj(void) {
    Habitante h_in = cria_habitante("444.444", "Carlos", "Despejado", 'M', "02/02/1980");
    set_habitante_endereco(h_in, "CEP-456", 'S', 20, ""); 
    insert_exHash(h_hab, "444.444", h_in);
    Quadra q_in = cria_quadra("CEP-456", 0.0, 0.0, 10.0, 10.0, "2.0px", "red", "red");
    insert_exHash(h_quad, "CEP-456", q_in);

    FILE* qry_file = fopen("teste.qry", "w");
    fprintf(qry_file, "dspj 444.444\n");
    fclose(qry_file);

    processa_arquivo_qry("teste.qry", "teste_saida.txt", "teste_saida.svg", h_hab, h_quad);

    Habitante h_out = (Habitante) search_exHash(h_hab, "444.444");
    TEST_ASSERT_NOT_NULL(h_out);
    TEST_ASSERT_TRUE(is_sem_teto(h_out)); 
}

void test_ComandoRq(void) {
    Quadra q_in = cria_quadra("CEP-DEMOLIDO", 0, 0, 10, 10, "1.5px", "green", "black");
    insert_exHash(h_quad, "CEP-DEMOLIDO", q_in);

    Habitante h_in = cria_habitante("555.555", "Ana", "Triste", 'F', "01/01/2001");
    set_habitante_endereco(h_in, "CEP-DEMOLIDO", 'L', 5, "");
    insert_exHash(h_hab, "555.555", h_in);

    FILE* qry_file = fopen("teste.qry", "w");
    fprintf(qry_file, "rq CEP-DEMOLIDO\n");
    fclose(qry_file);

    processa_arquivo_qry("teste.qry", "teste_saida.txt", "teste_saida.svg", h_hab, h_quad);

    Quadra q_out = (Quadra) search_exHash(h_quad, "CEP-DEMOLIDO");
    TEST_ASSERT_NULL(q_out);

    Habitante h_out = (Habitante) search_exHash(h_hab, "555.555");
    TEST_ASSERT_TRUE(is_sem_teto(h_out));
}

void test_ComandosPq_e_Censo(void) {
    Habitante h1 = cria_habitante("1", "H1", "Um", 'M', "1"); set_habitante_endereco(h1, "CEP-TESTE", 'N', 1, ""); insert_exHash(h_hab, "1", h1);
    Habitante h2 = cria_habitante("2", "H2", "Dois", 'F', "2"); set_habitante_endereco(h2, "CEP-TESTE", 'S', 2, ""); insert_exHash(h_hab, "2", h2);
    Habitante h3 = cria_habitante("3", "H3", "SemTeto", 'M', "3"); insert_exHash(h_hab, "3", h3); 
    
    Quadra q = cria_quadra("CEP-TESTE", 0, 0, 10, 10, "1.0px", "none", "black");
    insert_exHash(h_quad, "CEP-TESTE", q);

    FILE* qry_file = fopen("teste.qry", "w");
    fprintf(qry_file, "pq CEP-TESTE\ncenso\n");
    fclose(qry_file);

    processa_arquivo_qry("teste.qry", "teste_saida.txt", "teste_saida.svg", h_hab, h_quad);

    FILE* saida_txt = fopen("teste_saida.txt", "r");
    TEST_ASSERT_NOT_NULL(saida_txt);
    
    char buffer[2048] = {0};
    fread(buffer, 1, sizeof(buffer)-1, saida_txt);
    fclose(saida_txt);

    TEST_ASSERT_NOT_NULL(strstr(buffer, "Total na quadra: 2")); 
    TEST_ASSERT_NOT_NULL(strstr(buffer, "total de habitantes: 3"));
    TEST_ASSERT_NOT_NULL(strstr(buffer, "total de moradores: 2"));
    TEST_ASSERT_NOT_NULL(strstr(buffer, "total de sem-tetos: 1"));
}

void test_ComandoH_Interrogacao(void) {
    Habitante h_in = cria_habitante("777.777", "Bruce", "Wayne", 'M', "19/02/1939");
    insert_exHash(h_hab, "777.777", h_in);

    FILE* qry_file = fopen("teste.qry", "w");
    fprintf(qry_file, "h? 777.777\n");
    fclose(qry_file);

    processa_arquivo_qry("teste.qry", "teste_saida.txt", "teste_saida.svg", h_hab, h_quad);

    FILE* saida_txt = fopen("teste_saida.txt", "r");
    char buffer[1024] = {0};
    fread(buffer, 1, sizeof(buffer)-1, saida_txt);
    fclose(saida_txt);

    TEST_ASSERT_NOT_NULL(strstr(buffer, "Bruce Wayne"));
}


int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_ComandoNasc);
    RUN_TEST(test_ComandoRip);
    RUN_TEST(test_ComandoMud);
    RUN_TEST(test_ComandoDspj);
    RUN_TEST(test_ComandoRq);
    RUN_TEST(test_ComandosPq_e_Censo);
    RUN_TEST(test_ComandoH_Interrogacao);
    
    return UNITY_END();
}
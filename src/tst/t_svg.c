#include "../unity.h"
#include "../svg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void setUp(void) {
    remove("teste_saida.svg");
}

void tearDown(void) {
    remove("teste_saida.svg");
}

void test_AbreEFechaSVG(void) {
    FILE* f = abre_svg("teste_saida.svg");
    TEST_ASSERT_NOT_NULL(f);
    fecha_svg(f);
    FILE* l = fopen("teste_saida.svg", "r");
    char buffer[1024] = {0};
    fread(buffer, 1, sizeof(buffer) - 1, l);
    fclose(l);

    TEST_ASSERT_NOT_NULL(strstr(buffer, "<svg"));
    TEST_ASSERT_NOT_NULL(strstr(buffer, "</svg>"));
}

void test_DesenhaQuadra(void) {
    FILE* f = abre_svg("teste_saida.svg");
    svg_quadra(f, 10.0, 20.0, 50.0, 50.0, "red", "black", "2px");
    fecha_svg(f);

    FILE* l = fopen("teste_saida.svg", "r");
    char buffer[1024] = {0};
    fread(buffer, 1, sizeof(buffer) - 1, l);
    fclose(l);

    TEST_ASSERT_NOT_NULL(strstr(buffer, "<rect"));
    TEST_ASSERT_NOT_NULL(strstr(buffer, "x=\"10.000000\"")); 
    TEST_ASSERT_NOT_NULL(strstr(buffer, "fill=\"red\""));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_AbreEFechaSVG);
    RUN_TEST(test_DesenhaQuadra);
    return UNITY_END();
}
#include "geo.h"
#include "quadra.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Funções Auxiliares Estáticas ---

static void comando_cq(const char *linha, char *sw, char *fill, char *stroke) {
    sscanf(linha, "%*s %s %s %s", sw, fill, stroke);
}
static void comando_q(const char *linha, exHash hash_quadras, const char *sw, const char *fill, const char *stroke) {
    char cep[32];
    double x, y, w, h;

    sscanf(linha, "%*s %s %lf %lf %lf %lf", cep, &x, &y, &w, &h);
    Quadra q = cria_quadra(cep, x, y, w, h, sw, fill, stroke);
    
    if (q != NULL) {
        if (!insert_exHash(hash_quadras, cep, q)) {
            printf("Aviso: Quadra %s ignorada (ja existe).\n", cep);
        }
        free_quadra(q); 
    }
}

// --- Função Principal ---

void processa_arquivo_geo(const char *caminho_arquivo, exHash hash_quadras) {
    FILE *f_geo = fopen(caminho_arquivo, "r");
    if (!f_geo) {
        printf("Erro ao abrir arquivo GEO: %s\n", caminho_arquivo);
        return;
    }

    char espessura_atual[16] = "1.0px"; 
    char cor_preenchimento[32] = "white";
    char cor_borda[32] = "black";

    char linha[256];
    while (fgets(linha, sizeof(linha), f_geo)) {
        char comando[3];
        if (sscanf(linha, "%2s", comando) != 1) continue;

        if (strcmp(comando, "cq") == 0) {
            comando_cq(linha, espessura_atual, cor_preenchimento, cor_borda);
        } 
        else if (strcmp(comando, "q") == 0) {
            comando_q(linha, hash_quadras, espessura_atual, cor_preenchimento, cor_borda);
        }
    }

    fclose(f_geo);
}
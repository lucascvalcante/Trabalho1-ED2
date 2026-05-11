#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exHash.h"    
#include "geo.h"
#include "pm.h"
#include "qry.h"
#include "svg.h" 
#include "habitante.h"

#define PATH_SIZE 1024

static void monta_caminho(char* destino, const char* dir, const char* arquivo) {
    if (dir != NULL && strlen(dir) > 0) {
        if (dir[strlen(dir) - 1] == '/') sprintf(destino, "%s%s", dir, arquivo);
        else sprintf(destino, "%s/%s", dir, arquivo);
    } else {
        strcpy(destino, arquivo);
    }
}

static void extrair_nome_base(const char *caminho, char *nome_base) {
    const char *ultimo_slash = strrchr(caminho, '/');
    const char *nome_com_ext = ultimo_slash ? ultimo_slash + 1 : caminho;
    strcpy(nome_base, nome_com_ext);

    char* ponto_ext = strrchr(nome_base, '.');
    if (ponto_ext) {
        *ponto_ext = '\0'; 
    }
}

int main(int argc, char *argv[]) {
    char *dir_in = "";
    char *dir_out = NULL;
    char *arq_geo = NULL;
    char *arq_qry = NULL;
    char *arq_pm = NULL;


    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-e") == 0 && i + 1 < argc) dir_in = argv[++i];
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) dir_out = argv[++i];
        else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) arq_geo = argv[++i];
        else if (strcmp(argv[i], "-q") == 0 && i + 1 < argc) arq_qry = argv[++i];
        else if (strcmp(argv[i], "-pm") == 0 && i + 1 < argc) arq_pm = argv[++i];
    }

    if (arq_geo == NULL || dir_out == NULL) {
        fprintf(stderr, "ERRO: Parametros -f e -o sao obrigatorios!\n");
        return 1;
    }

    char nome_base_geo[256];
    extrair_nome_base(arq_geo, nome_base_geo);

    char path_geo_completo[PATH_SIZE];
    monta_caminho(path_geo_completo, dir_in, arq_geo);

    char path_hash_quad[PATH_SIZE], path_hash_hab[PATH_SIZE];
    monta_caminho(path_hash_quad, dir_out, "quadras.hf");
    monta_caminho(path_hash_hab, dir_out, "habitantes.hf");

    exHash h_quad = init_exHash(path_hash_quad, 32, get_tamanho_quadra()); 
    exHash h_hab = init_exHash(path_hash_hab, 32, get_habitante_size());

    printf("[*] Processando GEO: %s\n", path_geo_completo);
    processa_arquivo_geo(path_geo_completo, h_quad); 

    char path_svg_geo[PATH_SIZE];
    sprintf(path_svg_geo, "%s/%s.svg", dir_out, nome_base_geo);

    printf("[*] Gerando SVG do mapa base: %s\n", path_svg_geo);
    gerar_svg_geo(path_svg_geo, h_quad); 

    if (arq_pm != NULL) {
        char path_pm_completo[PATH_SIZE];
        monta_caminho(path_pm_completo, dir_in, arq_pm);
        printf("[*] Processando PM: %s\n", path_pm_completo);
        processa_arquivo_pm(path_pm_completo, h_hab); 
    }

    if (arq_qry != NULL) {
        char nome_base_qry[256];
        extrair_nome_base(arq_qry, nome_base_qry);
        char path_qry_completo[PATH_SIZE];
        monta_caminho(path_qry_completo, dir_in, arq_qry);
        char path_txt_out[PATH_SIZE], path_svg_out[PATH_SIZE];
        snprintf(path_txt_out, PATH_SIZE, "%s/%s-%s.txt", dir_out, nome_base_geo, nome_base_qry);
        snprintf(path_svg_out, PATH_SIZE, "%s/%s-%s.svg", dir_out, nome_base_geo, nome_base_qry);
        printf("[*] Executando consultas QRY: %s\n", arq_qry);
        processa_arquivo_qry(path_qry_completo, path_txt_out, path_svg_out, h_hab, h_quad);
    }

    char path_hfd_quad[PATH_SIZE], path_hfd_hab[PATH_SIZE];
    monta_caminho(path_hfd_quad, dir_out, "quadras.hfd");
    monta_caminho(path_hfd_hab, dir_out, "habitantes.hfd");

    printf("[*] Gerando arquivos de dump (.hfd)...\n");
    dump_exHash(h_quad, path_hfd_quad);
    dump_exHash(h_hab, path_hfd_hab);

    close_exHash(h_quad);
    close_exHash(h_hab);

    printf("Finalizado com sucesso.\n");
    return 0;
}
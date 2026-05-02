#include "qry.h"
#include "quadra.h"
#include "habitante.h"
#include "svg.h"
#include "exHash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Funções auxiliares estáticas: --- //

static void calcula_coords_endereco(Quadra q, char face, int num, double* x, double* y) {
    if (face == 'S') {
        *x = get_quadra_x(q) + num;
        *y = get_quadra_y(q);
    } 
    else if (face == 'N') {
        *x = get_quadra_x(q) + num;
        *y = get_quadra_y(q) + get_quadra_h(q);
    } 
    else if (face == 'L') {
        *x = get_quadra_x(q);
        *y = get_quadra_y(q) + num;
    } 
    else if (face == 'O') {
        *x = get_quadra_x(q) + get_quadra_w(q);
        *y = get_quadra_y(q) + num;
    } 
    else {
        *x = get_quadra_x(q);
        *y = get_quadra_y(q);
    }
}

// -- Callbacks: -- // 

typedef struct {
    const char* cep_alvo;
    FILE* txt;
    exHash h_hab;
} AuxRQ;

static void cb_rq_semteto(const char* chave, void* dado, void* extra) {
    AuxRQ* aux = (AuxRQ*)extra;
    Habitante h = (Habitante)dado;
    
    if (!is_sem_teto(h) && strcmp(get_habitante_cep(h), aux->cep_alvo) == 0) {
        fprintf(aux->txt, "- CPF: %s | Nome: %s %s\n", chave, get_habitante_nome(h), get_habitante_sobrenome(h));
        set_habitante_sem_teto(h); 
        update_exHash(aux->h_hab, chave, h);
    }
}

typedef struct {
    const char* cep_alvo;
    int total, c_n, c_s, c_l, c_o;
} AuxPQ;

static void cb_conta_quadra(const char* chave, void* dado, void* extra) {
    (void)chave;
    AuxPQ* aux = (AuxPQ*)extra;
    Habitante h = (Habitante)dado;
    
    if (!is_sem_teto(h) && strcmp(get_habitante_cep(h), aux->cep_alvo) == 0) {
        aux->total++;
        char face = get_habitante_face(h);
        if (face == 'N') aux->c_n++;
        else if (face == 'S') aux->c_s++;
        else if (face == 'L') aux->c_l++;
        else if (face == 'O') aux->c_o++;
    }
}

typedef struct {
    int total_hab, hab_h, hab_m;
    int moradores, mor_h, mor_m;
    int sem_teto, st_h, st_m;
} AuxCenso;

static void cb_censo(const char* chave, void* dado, void* extra) {
    (void)chave;
    AuxCenso* aux = (AuxCenso*)extra;
    Habitante h = (Habitante)dado;

    char sexo = get_habitante_sexo(h);
    bool is_h = (sexo == 'M');
    bool is_st = is_sem_teto(h); 

    aux->total_hab++;
    if (is_h) aux->hab_h++; else aux->hab_m++;

    if (is_st) {
        aux->sem_teto++;
        if (is_h) aux->st_h++; else aux->st_m++;
    } else {
        aux->moradores++;
        if (is_h) aux->mor_h++; else aux->mor_m++;
    }
}

static void cmd_h_interrogacao(FILE* txt, exHash h_hab, const char* cpf) {
    fprintf(txt, "h? %s\n", cpf);
    Habitante h = (Habitante) search_exHash(h_hab, cpf); 
    if (h) {
        fprintf(txt, "Habitante: %s %s | Sexo: %c | Nasc: %s\n", 
                get_habitante_nome(h), get_habitante_sobrenome(h), get_habitante_sexo(h), get_habitante_nascimento(h));
        
        if (!is_sem_teto(h)) {
            fprintf(txt, "Endereco: CEP %s, Face %c, Num %.0f, Cmpl: %s\n", 
                    get_habitante_cep(h), get_habitante_face(h), get_habitante_numero(h), get_habitante_complemento(h));
        } else {
            fprintf(txt, "Status: Sem-teto.\n");
        }

        free(h);

    } else {
        fprintf(txt, "Erro: Habitante nao encontrado.\n");
    }
    fprintf(txt, "\n");
}

static void cmd_nasc(FILE* txt, exHash h_hab, const char* cpf, const char* nome, const char* sobrenome, char sexo, const char* data_nasc) {
    fprintf(txt, "nasc %s %s\n", cpf, nome);
    Habitante h = cria_habitante(cpf, nome, sobrenome, sexo, data_nasc);
    
    if (insert_exHash(h_hab, cpf, h)) {
        fprintf(txt, "Habitante cadastrado com sucesso.\n");
    } else {
        fprintf(txt, "Erro: CPF ja existente.\n");
    }

    free_habitante(h); 
    fprintf(txt, "\n");
}

static void cmd_rip(FILE* txt, FILE* svg, exHash h_hab, exHash h_quadras, const char* cpf) {
    fprintf(txt, "rip %s\n", cpf);
    Habitante h = (Habitante) search_exHash(h_hab, cpf);
    if (h) {
        fprintf(txt, "Falecimento: %s %s | Sexo: %c | Nasc: %s\n", get_habitante_nome(h), get_habitante_sobrenome(h), get_habitante_sexo(h), get_habitante_nascimento(h));
        
        if (!is_sem_teto(h)) {
            const char* cep = get_habitante_cep(h);
            fprintf(txt, "Morava em: CEP %s, Face %c, Num %.0f\n", cep, get_habitante_face(h), get_habitante_numero(h));
            
            Quadra q = (Quadra) search_exHash(h_quadras, cep);
            if (q) {
                double cx, cy;
                calcula_coords_endereco(q, get_habitante_face(h), get_habitante_numero(h), &cx, &cy);
                svg_marca_cruz(svg, cx, cy); 
                free_quadra(q);
            }
        }
        remove_exHash(h_hab, cpf); 
        free_habitante(h); 
    }
    fprintf(txt, "\n");
}

static void cmd_mud(FILE* txt, FILE* svg, exHash h_hab, exHash h_quadras, const char* cpf, const char* novo_cep, char face, int num, const char* cmpl) {
    fprintf(txt, "mud %s para %s\n", cpf, novo_cep);
    Habitante h = (Habitante) search_exHash(h_hab, cpf);
    if (h) {
        Quadra q = (Quadra) search_exHash(h_quadras, novo_cep);
        if (q) {
            double cx, cy;
            calcula_coords_endereco(q, face, num, &cx, &cy);
            
            svg_quadra(svg, cx-5, cy-5, 10, 10, "none", "red", "1px");
            svg_texto(svg, cx, cy+3, cpf, "black", "middle"); 
            
            set_habitante_endereco(h, novo_cep, face, num, cmpl);
            update_exHash(h_hab, cpf, h);
            
            fprintf(txt, "Mudanca realizada com sucesso.\n");
            free_quadra(q);

        } else {
            fprintf(txt, "Erro: Nova quadra nao existe.\n");
        }

        free_habitante(h);
    }
    fprintf(txt, "\n");
}

static void cmd_dspj(FILE* txt, FILE* svg, exHash h_hab, exHash h_quadras, const char* cpf) {
    fprintf(txt, "dspj %s\n", cpf);
    Habitante h = (Habitante) search_exHash(h_hab, cpf);
    
    if (h) {
        if (!is_sem_teto(h)) {
            const char* cep = get_habitante_cep(h);
            Quadra q = (Quadra) search_exHash(h_quadras, cep);
            
            if (q) {
                double cx, cy;
                calcula_coords_endereco(q, get_habitante_face(h), get_habitante_numero(h), &cx, &cy);
                svg_marca_circulo(svg, cx, cy); 
                free_quadra(q); 
            }
            
            fprintf(txt, "Despejado: %s (morava em %s %.0f)\n", get_habitante_nome(h), cep, get_habitante_numero(h));
            
            set_habitante_sem_teto(h);
            update_exHash(h_hab, cpf, h);
        } else {
            fprintf(txt, "Aviso: Habitante ja era sem-teto.\n");
        }
        free_habitante(h); 
    }
    fprintf(txt, "\n");
}

static void cmd_rq(FILE* txt, FILE* svg, exHash h_hab, exHash h_quadras, const char* cep) {
    fprintf(txt, "rq %s\n", cep);
    Quadra q = (Quadra) search_exHash(h_quadras, cep);
    if (q) {
        svg_marca_x(svg, get_quadra_x(q), get_quadra_y(q)); 
        
        remove_exHash(h_quadras, cep);
        free_quadra(q); 

        fprintf(txt, "Moradores que viraram sem-teto:\n");        
        AuxRQ aux = {cep, txt, h_hab};
        foreach_exHash(h_hab, cb_rq_semteto, &aux);

    } else {
        fprintf(txt, "Quadra nao encontrada.\n");
    }
    fprintf(txt, "\n");
}

static void cmd_pq(FILE* txt, FILE* svg, exHash h_hab, exHash h_quadras, const char* cep) {
    fprintf(txt, "pq %s\n", cep);
    Quadra q = (Quadra) search_exHash(h_quadras, cep);
    if (!q) {
        fprintf(txt, "Quadra nao encontrada.\n\n");
        return;
    }

    AuxPQ aux = {cep, 0, 0, 0, 0, 0};
    foreach_exHash(h_hab, cb_conta_quadra, &aux);

    double qx = get_quadra_x(q), qy = get_quadra_y(q);
    double qw = get_quadra_w(q), qh = get_quadra_h(q);
    
    char buf[16];
    
sprintf(buf, "%d", aux.total);
    svg_texto(svg, qx + (qw/2.0), qy + (qh/2.0), buf, "black", "middle");

    sprintf(buf, "%d", aux.c_s); 
    svg_texto(svg, qx + (qw/2.0), qy - 5, buf, "blue", "middle");
    sprintf(buf, "%d", aux.c_n); 
    svg_texto(svg, qx + (qw/2.0), qy + qh + 15, buf, "blue", "middle");
    sprintf(buf, "%d", aux.c_l); 
    svg_texto(svg, qx - 10, qy + (qh/2.0), buf, "blue", "end");
    sprintf(buf, "%d", aux.c_o); 
    svg_texto(svg, qx + qw + 10, qy + (qh/2.0), buf, "blue", "start");

    fprintf(txt, "Total na quadra: %d (N:%d S:%d L:%d O:%d)\n\n", aux.total, aux.c_n, aux.c_s, aux.c_l, aux.c_o);

    free_quadra(q);
}

static void cmd_censo(FILE* txt, exHash h_hab) {
    fprintf(txt, "censo\n");
    fprintf(txt, "Estatisticas de Bitnopolis:\n");

    AuxCenso aux = {0}; 
    foreach_exHash(h_hab, cb_censo, &aux);

    if (aux.total_hab == 0) { 
        fprintf(txt, "Cidade Vazia. Nao ha dados para o censo.\n\n"); 
        return; 
    }    
    double prop_moradores = (double)aux.moradores / aux.total_hab;
    
    double perc_hab_h = ((double)aux.hab_h / aux.total_hab) * 100.0;
    double perc_hab_m = ((double)aux.hab_m / aux.total_hab) * 100.0;    
    double perc_mor_h = aux.moradores > 0 ? ((double)aux.mor_h / aux.moradores) * 100.0 : 0.0;
    double perc_mor_m = aux.moradores > 0 ? ((double)aux.mor_m / aux.moradores) * 100.0 : 0.0;
    double perc_st_h = aux.sem_teto > 0 ? ((double)aux.st_h / aux.sem_teto) * 100.0 : 0.0;
    double perc_st_m = aux.sem_teto > 0 ? ((double)aux.st_m / aux.sem_teto) * 100.0 : 0.0;

    fprintf(txt, "- Numero total de habitantes: %d\n", aux.total_hab);
    fprintf(txt, "- Numero total de moradores: %d\n", aux.moradores);
    fprintf(txt, "- Proporcao moradores/habitantes: %.2f\n", prop_moradores);
    fprintf(txt, "- Numero de homens: %d\n", aux.hab_h);
    fprintf(txt, "- Numero de mulheres: %d\n", aux.hab_m);
    fprintf(txt, "- %% de habitantes homens: %.2f%%\n", perc_hab_h);
    fprintf(txt, "- %% habitantes mulheres: %.2f%%\n", perc_hab_m);
    fprintf(txt, "- %% de moradores homens: %.2f%%\n", perc_mor_h);
    fprintf(txt, "- %% de moradores mulheres: %.2f%%\n", perc_mor_m);
    fprintf(txt, "- Numero total de sem-tetos: %d\n", aux.sem_teto);
    fprintf(txt, "- %% sem-tetos homens: %.2f%%\n", perc_st_h);
    fprintf(txt, "- %% sem-tetos mulheres: %.2f%%\n", perc_st_m);
    fprintf(txt, "\n");
}

// --- Função principal: --- //

void processa_arquivo_qry(const char* caminho_qry, const char* caminho_txt, const char* caminho_svg, exHash h_hab, exHash h_quadras) {
    FILE* f_qry = fopen(caminho_qry, "r");
    FILE* f_txt = fopen(caminho_txt, "w");
    FILE* f_svg = abre_svg(caminho_svg);

    if (!f_qry || !f_txt || !f_svg) {
        printf("Erro ao abrir arquivos do QRY.\n");
        if (f_qry) fclose(f_qry);
        if (f_txt) fclose(f_txt);
        if (f_svg) fecha_svg(f_svg);
        return;
    }

    foreach_exHash(h_quadras, escreve_retangulo_svg, f_svg);

    char linha[256];
    while (fgets(linha, sizeof(linha), f_qry)) {
        char comando[20];
        if (sscanf(linha, "%19s", comando) != 1) continue;

        if (strcmp(comando, "h?") == 0) {
            char cpf[32];
            sscanf(linha, "%*s %31s", cpf);
            cmd_h_interrogacao(f_txt, h_hab, cpf);
        } 
        else if (strcmp(comando, "nasc") == 0) {
            char cpf[32], nome[64], sob[64], sexo, nasc[20];
            sscanf(linha, "%*s %31s %63s %63s %c %19s", cpf, nome, sob, &sexo, nasc);
            cmd_nasc(f_txt, h_hab, cpf, nome, sob, sexo, nasc);
        }
        else if (strcmp(comando, "rip") == 0) {
            char cpf[32];
            sscanf(linha, "%*s %31s", cpf);
            cmd_rip(f_txt, f_svg, h_hab, h_quadras, cpf);
        }
        else if (strcmp(comando, "mud") == 0) {
            char cpf[32], cep[32], face, cmpl[64] = "";
            int num;
            sscanf(linha, "%*s %31s %31s %c %d %63[^\n]", cpf, cep, &face, &num, cmpl);
            cmd_mud(f_txt, f_svg, h_hab, h_quadras, cpf, cep, face, num, cmpl);
        }
        else if (strcmp(comando, "dspj") == 0) {
            char cpf[32];
            sscanf(linha, "%*s %31s", cpf);
            cmd_dspj(f_txt, f_svg, h_hab, h_quadras, cpf);
        }
        else if (strcmp(comando, "rq") == 0) {
            char cep[32];
            sscanf(linha, "%*s %31s", cep);
            cmd_rq(f_txt, f_svg, h_hab, h_quadras, cep);
        }
        else if (strcmp(comando, "pq") == 0) {
            char cep[32];
            sscanf(linha, "%*s %31s", cep);
            cmd_pq(f_txt, f_svg, h_hab, h_quadras, cep);
        }
        else if (strcmp(comando, "censo") == 0) {
            cmd_censo(f_txt, h_hab);
        }
    }

    fclose(f_qry);
    fclose(f_txt);
    fecha_svg(f_svg); 
}
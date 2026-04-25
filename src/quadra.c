#include "quadra.h"
#include <stdlib.h>
#include <string.h>

struct quadra_s {
    char cep[32];
    double x;
    double y;
    double w;
    double h;
    char sw[16];     
    char cfill[32];
    char cstrk[32];
};

Quadra cria_quadra(const char* cep, double x, double y, double w, double h, const char* sw, const char* cfill, const char* cstrk) {
    Quadra q = (Quadra) malloc(sizeof(struct quadra_s));
    if (q == NULL) return NULL;

    strncpy(q->cep, cep, 31);
    q->cep[31] = '\0';
    
    q->x = x;
    q->y = y;
    q->w = w;
    q->h = h;

    strncpy(q->sw, sw, 15);
    q->sw[15] = '\0';

    strncpy(q->cfill, cfill, 31);
    q->cfill[31] = '\0';

    strncpy(q->cstrk, cstrk, 31);
    q->cstrk[31] = '\0';

    return q;
}

const char* get_quadra_cep(Quadra q) {
    if (!q) return NULL;
    return q->cep;
}

double get_quadra_x(Quadra q){
    if (!q) return 0;
    return q->x;
}

double get_quadra_y(Quadra q) {
    if (!q) return 0;
    return q->y; 
}

double get_quadra_w(Quadra q) {
    if (!q) return 0;
    return q->w;
}


double get_quadra_h(Quadra q) {
    if (!q) return 0; 
    return q->h;
}

const char* get_quadra_cfill(Quadra q) {
    if (!q) return NULL;
    return q->cfill; 
}

const char* get_quadra_cstrk(Quadra q) {
    if (!q) return NULL;
    return q->cstrk;
}

const char* get_quadra_sw(Quadra q) {
    if (!q) return NULL; 
    return q->sw ; 
}


void get_quadra_ancora(Quadra q, double* ancora_x, double* ancora_y) {
    if (!q || !ancora_x || !ancora_y) return;
    *ancora_x = q->x + q->w;
    *ancora_y = q->y + q->h;
}

void free_quadra(Quadra q) {
    if (q) {
        free(q);
    }
}
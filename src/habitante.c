#include "habitante.h"
#include <stdlib.h>
#include <string.h>

struct habitante_s {
    char cpf[15];        
    char nome[50];
    char sobrenome[50];
    char sexo;
    char nascimento[11];  
    
    bool sem_teto;
    char cep[20];
    char face;
    double numero;
    char complemento[50];
};

size_t get_habitante_size() {
    return sizeof(struct habitante_s);
}

Habitante cria_habitante(const char* cpf, const char* nome, const char* sobrenome, char sexo, const char* nascimento) {
    Habitante h = (Habitante) calloc(1, sizeof(struct habitante_s));
    if (h == NULL) return NULL;
    if (cpf) strncpy(h->cpf, cpf, sizeof(h->cpf) - 1);
    if (nome) strncpy(h->nome, nome, sizeof(h->nome) - 1);
    if (sobrenome) strncpy(h->sobrenome, sobrenome, sizeof(h->sobrenome) - 1);
    
    h->sexo = sexo;
    
    if (nascimento) strncpy(h->nascimento, nascimento, sizeof(h->nascimento) - 1);

    h->sem_teto = true;
    return h;
}

void free_habitante(Habitante h) {
    if (h != NULL) {
        free(h);
    }
}

void set_habitante_endereco(Habitante h, const char* cep, char face, double numero, const char* complemento) {
    if (h == NULL) return;

    h->sem_teto = false;
    
    if (cep) strncpy(h->cep, cep, sizeof(h->cep) - 1);
    h->face = face;
    h->numero = numero;
    if (complemento) strncpy(h->complemento, complemento, sizeof(h->complemento) - 1);
}

void set_habitante_sem_teto(Habitante h) {
    if (h != NULL) {
        h->sem_teto = true;
        h->cep[0] = '\0';
        h->face = '\0';
        h->numero = 0.0;
        h->complemento[0] = '\0';
    }
}

bool is_sem_teto(Habitante h) {
    if (h == NULL) return true; 
    return h->sem_teto;
}

const char* get_habitante_cpf(Habitante h) { return h ? h->cpf : NULL; }
const char* get_habitante_nome(Habitante h) { return h ? h->nome : NULL; }
const char* get_habitante_sobrenome(Habitante h) { return h ? h->sobrenome : NULL; }
char get_habitante_sexo(Habitante h) { return h ? h->sexo : '\0'; }
const char* get_habitante_nascimento(Habitante h) { return h ? h->nascimento : NULL; }

const char* get_habitante_cep(Habitante h) { 
    return (h && !h->sem_teto) ? h->cep : NULL; 
}
char get_habitante_face(Habitante h) { 
    return (h && !h->sem_teto) ? h->face : '\0'; 
}
double get_habitante_numero(Habitante h) { 
    return (h && !h->sem_teto) ? h->numero : 0.0; 
}
const char* get_habitante_complemento(Habitante h) { 
    return (h && !h->sem_teto) ? h->complemento : NULL; 
}

void print_habitante_info(FILE* txt, Habitante h) {
    if (txt == NULL || h == NULL) return;

    fprintf(txt, "--------------------------------------------------\n");
    fprintf(txt, "Cidadão: %s %s\n", h->nome, h->sobrenome);
    fprintf(txt, "CPF: %s | Sexo: %c | Nasc: %s\n", h->cpf, h->sexo, h->nascimento);
    
    if (h->sem_teto) {
        fprintf(txt, "Situação de Moradia: Sem-teto\n");
    } else {
        fprintf(txt, "Endereço: Quadra %s, Face %c, Num %.2f", h->cep, h->face, h->numero);
        if (strlen(h->complemento) > 0) {
            fprintf(txt, " - %s", h->complemento);
        }
        fprintf(txt, "\n");
    }
    fprintf(txt, "--------------------------------------------------\n");
}
#include "exHash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHAVE 32

typedef struct {
    uint32_t profundidade_local;
    uint32_t qtd_registros;
} CabecalhoBucket;

typedef struct {
    char chave[MAX_CHAVE];
    bool ocupado;
} Registro;

struct exHash_s {
    FILE* arquivo_dados;         
    FILE* arquivo_dir;            
    char nome_arquivo_base[256];  
    
    uint32_t profundidade_global; 
    uint32_t tamanho_bucket;      
    size_t tamanho_dado;          
    size_t tamanho_bucket_bytes;  
    
    long* diretorio;              
};

///---Funções auxiliares estáticas: ---///

static uint32_t hash_string(const char* str) {
    uint32_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; 
    }
    return hash;
}

static uint32_t get_bits(uint32_t hash, uint32_t profundidade) {
    if (profundidade == 0) return 0;
    return hash & ((1 << profundidade) - 1);
}

static long criar_bucket_vazio(exHash h, uint32_t profundidade_local) {
    CabecalhoBucket cabecalho = {profundidade_local, 0};

    fseek(h->arquivo_dados, 0, SEEK_END);
    long offset = ftell(h->arquivo_dados);  

    fwrite(&cabecalho, sizeof(CabecalhoBucket), 1, h->arquivo_dados);
    size_t espaco_vazio = h->tamanho_bucket_bytes - sizeof(CabecalhoBucket);
    void* buffer = calloc(1, espaco_vazio);
    fwrite(buffer, 1, espaco_vazio, h->arquivo_dados);
    free(buffer);
    
    return offset;
}

///Funções principais: ---///

exHash init_exHash(const char* arquivo, uint32_t tamanho_bucket, size_t tamanho_dado) {
    exHash h = (exHash) malloc(sizeof(struct exHash_s));
    if (!h) return NULL;

    strncpy(h->nome_arquivo_base, arquivo, 255);
    h->arquivo_dados = fopen(arquivo, "w+b");
    char nome_dir[300];
    snprintf(nome_dir, sizeof(nome_dir), "%s.dir", arquivo);
    h->arquivo_dir = fopen(nome_dir, "w+b");

    if (!h->arquivo_dados || !h->arquivo_dir) {
        free(h);
        return NULL;
    }

    h->profundidade_global = 0;
    h->tamanho_bucket = tamanho_bucket;
    h->tamanho_dado = tamanho_dado;
        size_t tamanho_registro_completo = sizeof(Registro) + tamanho_dado;
    h->tamanho_bucket_bytes = sizeof(CabecalhoBucket) + (tamanho_bucket * tamanho_registro_completo);
    h->diretorio = (long*) malloc(sizeof(long) * 1);
    h->diretorio[0] = criar_bucket_vazio(h, 0);

    return h;
}

void close_exHash(exHash h) {
    if (!h) return;

    rewind(h->arquivo_dir);
    fwrite(&h->profundidade_global, sizeof(uint32_t), 1, h->arquivo_dir);
    fwrite(&h->tamanho_bucket, sizeof(uint32_t), 1, h->arquivo_dir);
    fwrite(&h->tamanho_dado, sizeof(size_t), 1, h->arquivo_dir);

    uint32_t tamanho_dir = 1 << h->profundidade_global;
    fwrite(h->diretorio, sizeof(long), tamanho_dir, h->arquivo_dir);

    fclose(h->arquivo_dados);
    fclose(h->arquivo_dir);
    free(h->diretorio);
    free(h);
}

void* search_exHash(exHash h, const char* chave) {
    uint32_t hash = hash_string(chave);
    uint32_t indice_dir = get_bits(hash, h->profundidade_global);
    long offset_bucket = h->diretorio[indice_dir];
    fseek(h->arquivo_dados, offset_bucket, SEEK_SET);
    
    CabecalhoBucket cabecalho;
    fread(&cabecalho, sizeof(CabecalhoBucket), 1, h->arquivo_dados);

    size_t tamanho_registro_completo = sizeof(Registro) + h->tamanho_dado;
    void* buffer_registro = malloc(tamanho_registro_completo);

    for (uint32_t i = 0; i < h->tamanho_bucket; i++) {
        fread(buffer_registro, tamanho_registro_completo, 1, h->arquivo_dados);
        Registro* reg = (Registro*) buffer_registro;

        if (reg->ocupado && strcmp(reg->chave, chave) == 0) {
            void* dado_encontrado = malloc(h->tamanho_dado);
            memcpy(dado_encontrado, (char*)buffer_registro + sizeof(Registro), h->tamanho_dado);
            free(buffer_registro);
            return dado_encontrado;
        }
    }

    free(buffer_registro);
    return NULL;
}

bool remove_exHash(exHash h, const char* chave) {
    uint32_t hash = hash_string(chave);
    uint32_t indice_dir = get_bits(hash, h->profundidade_global);
    long offset_bucket = h->diretorio[indice_dir];

    fseek(h->arquivo_dados, offset_bucket, SEEK_SET);
    
    CabecalhoBucket cabecalho;
    fread(&cabecalho, sizeof(CabecalhoBucket), 1, h->arquivo_dados);

    size_t tamanho_registro_completo = sizeof(Registro) + h->tamanho_dado;
    void* buffer_registro = malloc(tamanho_registro_completo);

    for (uint32_t i = 0; i < h->tamanho_bucket; i++) {
        long pos_registro = ftell(h->arquivo_dados);
        fread(buffer_registro, tamanho_registro_completo, 1, h->arquivo_dados);
        Registro* reg = (Registro*) buffer_registro;

        if (reg->ocupado && strcmp(reg->chave, chave) == 0) {

            reg->ocupado = false;
            fseek(h->arquivo_dados, pos_registro, SEEK_SET);
            fwrite(buffer_registro, tamanho_registro_completo, 1, h->arquivo_dados);
            cabecalho.qtd_registros--;
            fseek(h->arquivo_dados, offset_bucket, SEEK_SET);
            fwrite(&cabecalho, sizeof(CabecalhoBucket), 1, h->arquivo_dados);

            free(buffer_registro);
            return true;
        }
    }

    free(buffer_registro);
    return false;
}

bool insert_exHash(exHash h, const char* chave, void* dado) {
    void* busca = search_exHash(h, chave);
    if (busca != NULL) {
        free(busca);
        return false; 
    }

    uint32_t hash = hash_string(chave);
    uint32_t indice_dir = get_bits(hash, h->profundidade_global);
    long offset_bucket = h->diretorio[indice_dir];

    fseek(h->arquivo_dados, offset_bucket, SEEK_SET);
    CabecalhoBucket cabecalho;
    fread(&cabecalho, sizeof(CabecalhoBucket), 1, h->arquivo_dados);

    size_t tamanho_registro_completo = sizeof(Registro) + h->tamanho_dado;

    if (cabecalho.qtd_registros < h->tamanho_bucket) {
        void* buffer_registro = malloc(tamanho_registro_completo);
        
        fseek(h->arquivo_dados, offset_bucket + sizeof(CabecalhoBucket), SEEK_SET);
        for (uint32_t i = 0; i < h->tamanho_bucket; i++) {
            long pos = ftell(h->arquivo_dados);
            fread(buffer_registro, tamanho_registro_completo, 1, h->arquivo_dados);
            Registro* reg = (Registro*) buffer_registro;
            
            if (!reg->ocupado) {
                strncpy(reg->chave, chave, MAX_CHAVE - 1);
                reg->chave[MAX_CHAVE - 1] = '\0';
                reg->ocupado = true;
                memcpy((char*)buffer_registro + sizeof(Registro), dado, h->tamanho_dado);
                
                fseek(h->arquivo_dados, pos, SEEK_SET);
                fwrite(buffer_registro, tamanho_registro_completo, 1, h->arquivo_dados);
                
                cabecalho.qtd_registros++;
                fseek(h->arquivo_dados, offset_bucket, SEEK_SET);
                fwrite(&cabecalho, sizeof(CabecalhoBucket), 1, h->arquivo_dados);
                
                free(buffer_registro);
                return true;
            }
        }
        free(buffer_registro);
    } 
    
    if (cabecalho.profundidade_local == h->profundidade_global) {
        uint32_t tamanho_antigo = 1 << h->profundidade_global;
        uint32_t novo_tamanho = 1 << (h->profundidade_global + 1);
        
        h->diretorio = realloc(h->diretorio, sizeof(long) * novo_tamanho);
        for (uint32_t i = 0; i < tamanho_antigo; i++) {
            h->diretorio[i + tamanho_antigo] = h->diretorio[i];
        }
        h->profundidade_global++;
    }

    void* backup_registros = malloc(h->tamanho_bucket * tamanho_registro_completo);
    fseek(h->arquivo_dados, offset_bucket + sizeof(CabecalhoBucket), SEEK_SET);
    fread(backup_registros, tamanho_registro_completo, h->tamanho_bucket, h->arquivo_dados);

    cabecalho.profundidade_local++;
    cabecalho.qtd_registros = 0;
    fseek(h->arquivo_dados, offset_bucket, SEEK_SET);
    fwrite(&cabecalho, sizeof(CabecalhoBucket), 1, h->arquivo_dados);
    
    void* espaco_vazio = calloc(h->tamanho_bucket, tamanho_registro_completo);
    fwrite(espaco_vazio, tamanho_registro_completo, h->tamanho_bucket, h->arquivo_dados);
    free(espaco_vazio);

    long novo_offset = criar_bucket_vazio(h, cabecalho.profundidade_local);

    uint32_t tamanho_dir = 1 << h->profundidade_global;
    uint32_t mascara_novo_bit = 1 << (cabecalho.profundidade_local - 1);

    for (uint32_t i = 0; i < tamanho_dir; i++) {
        if (h->diretorio[i] == offset_bucket) {
            if ((i & mascara_novo_bit) != 0) {
                h->diretorio[i] = novo_offset;
            }
        }
    }

    for (uint32_t i = 0; i < h->tamanho_bucket; i++) {
        Registro* reg = (Registro*)((char*)backup_registros + i * tamanho_registro_completo);
        if (reg->ocupado) {
            void* dado_antigo = (char*)reg + sizeof(Registro);
            insert_exHash(h, reg->chave, dado_antigo); 
        }
    }
    free(backup_registros);

    return insert_exHash(h, chave, dado);
}


void dump_exHash(exHash h, const char* arquivo_saida_hfd) {
    if (!h) return;
    FILE* saida = fopen(arquivo_saida_hfd, "w");
    if (!saida) return;

    fprintf(saida, "=== ESTRUTURA DO HASH EXTENSIVEL ===\n");
    fprintf(saida, "Profundidade Global: %u\n", h->profundidade_global);
    fprintf(saida, "Tamanho maximo do Bucket: %u registros\n", h->tamanho_bucket);
    
    uint32_t tamanho_dir = 1 << h->profundidade_global;
    fprintf(saida, "\n--- DIRETORIO (Tamanho: %u) ---\n", tamanho_dir);
    for (uint32_t i = 0; i < tamanho_dir; i++) {
        fprintf(saida, "Indice [%03u] -> Offset no Disco: %ld\n", i, h->diretorio[i]);
    }

    fprintf(saida, "====================================\n");
    fclose(saida);
}

exHash load_exHash(const char* arquivo) {
    char nome_dir[300];
    snprintf(nome_dir, sizeof(nome_dir), "%s.dir", arquivo);

    FILE* f_dados = fopen(arquivo, "r+b");
    FILE* f_dir = fopen(nome_dir, "r+b");

    if (!f_dados || !f_dir) {
        if (f_dados) fclose(f_dados);
        if (f_dir) fclose(f_dir);
        return NULL; 
    }

    exHash h = (exHash) malloc(sizeof(struct exHash_s));
    if (!h) return NULL;

    strncpy(h->nome_arquivo_base, arquivo, 255);
    h->arquivo_dados = f_dados;
    h->arquivo_dir = f_dir;

    rewind(h->arquivo_dir);
    fread(&h->profundidade_global, sizeof(uint32_t), 1, h->arquivo_dir);
    fread(&h->tamanho_bucket, sizeof(uint32_t), 1, h->arquivo_dir);
    fread(&h->tamanho_dado, sizeof(size_t), 1, h->arquivo_dir);

    size_t tamanho_registro_completo = sizeof(Registro) + h->tamanho_dado;
    h->tamanho_bucket_bytes = sizeof(CabecalhoBucket) + (h->tamanho_bucket * tamanho_registro_completo);
    uint32_t tamanho_dir = 1 << h->profundidade_global;
    h->diretorio = (long*) malloc(sizeof(long) * tamanho_dir);
    fread(h->diretorio, sizeof(long), tamanho_dir, h->arquivo_dir);

    return h;
}
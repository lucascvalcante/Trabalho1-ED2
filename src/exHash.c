#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "exhash.h"


typedef struct {
    uint32_t profundidade_local;
    uint32_t quantidade;
} BucketHeader;

typedef struct {
    FILE* arquivo_dados;
    FILE* arquivo_dir;
    uint32_t tamanho_bucket;
    size_t tamanho_dado;
    uint32_t profundidade_global;
    long* diretorio;
} ExHashManager;


#define TAMANHO_SLOT(manager) (sizeof(uint32_t) + manager->tamanho_dado)
#define TAMANHO_BUCKET_DISCO(manager) (sizeof(BucketHeader) + (manager->tamanho_bucket * TAMANHO_SLOT(manager)))


static uint32_t obter_indice(uint32_t chave, uint32_t profundidade) {
    if (profundidade == 0) return 0;
    return chave & ((1 << profundidade) - 1);
}

static void dobrar_diretorio(ExHashManager* manager) {
    uint32_t tamanho_antigo = 1 << manager->profundidade_global;
    uint32_t novo_tamanho = tamanho_antigo * 2;
    
    manager->diretorio = (long*) realloc(manager->diretorio, novo_tamanho * sizeof(long));
    
    for (uint32_t i = 0; i < tamanho_antigo; i++) {
        manager->diretorio[i + tamanho_antigo] = manager->diretorio[i];
    }
    manager->profundidade_global++;
}


exHash init_exHash(const char* arquivo, uint32_t tamanho_bucket, size_t tamanho_dado) {
    ExHashManager* manager = (ExHashManager*) malloc(sizeof(ExHashManager));
    if (!manager) return NULL;

    manager->tamanho_bucket = tamanho_bucket;
    manager->tamanho_dado = tamanho_dado;
    manager->profundidade_global = 0;

    char nome_dir[300];
    snprintf(nome_dir, sizeof(nome_dir), "%s_dir.bin", arquivo);

    manager->arquivo_dados = fopen(arquivo, "wb+");
    manager->arquivo_dir = fopen(nome_dir, "wb+");

    if (!manager->arquivo_dados || !manager->arquivo_dir) {
        free(manager);
        return NULL;
    }

    manager->diretorio = (long*) malloc(sizeof(long));
    manager->diretorio[0] = 0; 

    BucketHeader primeiro_bucket = {0, 0};
    fseek(manager->arquivo_dados, 0, SEEK_SET);
    fwrite(&primeiro_bucket, sizeof(BucketHeader), 1, manager->arquivo_dados);

    void* espaco = calloc(tamanho_bucket, TAMANHO_SLOT(manager));
    fwrite(espaco, TAMANHO_SLOT(manager), tamanho_bucket, manager->arquivo_dados);
    free(espaco);

    return (exHash) manager;
}

exHash load_exHash(const char* arquivo) {
    ExHashManager* manager = (ExHashManager*) malloc(sizeof(ExHashManager));
    if (!manager) return NULL;

    char nome_dir[300];
    snprintf(nome_dir, sizeof(nome_dir), "%s_dir.bin", arquivo);

    manager->arquivo_dados = fopen(arquivo, "rb+");
    manager->arquivo_dir = fopen(nome_dir, "rb+");

    if (!manager->arquivo_dados || !manager->arquivo_dir) {
        free(manager);
        return NULL;
    }

    fseek(manager->arquivo_dir, 0, SEEK_SET);
    fread(&(manager->tamanho_bucket), sizeof(uint32_t), 1, manager->arquivo_dir);
    fread(&(manager->tamanho_dado), sizeof(size_t), 1, manager->arquivo_dir);
    fread(&(manager->profundidade_global), sizeof(uint32_t), 1, manager->arquivo_dir);

    uint32_t tamanho_dir = 1 << manager->profundidade_global;
    manager->diretorio = (long*) malloc(tamanho_dir * sizeof(long));
    fread(manager->diretorio, sizeof(long), tamanho_dir, manager->arquivo_dir);

    return (exHash) manager;
}

bool insert_exHash(exHash hash, uint32_t chave, void* dado) {
    ExHashManager* manager = (ExHashManager*) hash;
    if (!manager) return false;

    uint32_t indice = obter_indice(chave, manager->profundidade_global);
    long offset_bucket = manager->diretorio[indice];

    BucketHeader header;
    fseek(manager->arquivo_dados, offset_bucket, SEEK_SET);
    fread(&header, sizeof(BucketHeader), 1, manager->arquivo_dados);

    uint32_t chave_lida;
    for (uint32_t i = 0; i < header.quantidade; i++) {
        fread(&chave_lida, sizeof(uint32_t), 1, manager->arquivo_dados);
        if (chave_lida == chave) return false; 
        fseek(manager->arquivo_dados, manager->tamanho_dado, SEEK_CUR); 
    }

    if (header.quantidade < manager->tamanho_bucket) {
        fseek(manager->arquivo_dados, offset_bucket + sizeof(BucketHeader) + (header.quantidade * TAMANHO_SLOT(manager)), SEEK_SET);
        fwrite(&chave, sizeof(uint32_t), 1, manager->arquivo_dados);
        fwrite(dado, manager->tamanho_dado, 1, manager->arquivo_dados);

        header.quantidade++;
        fseek(manager->arquivo_dados, offset_bucket, SEEK_SET);
        fwrite(&header, sizeof(BucketHeader), 1, manager->arquivo_dados);
        return true;
    }

    if (header.profundidade_local == manager->profundidade_global) {
        dobrar_diretorio(manager);
    }

    size_t tamanho_buffer = manager->tamanho_bucket * TAMANHO_SLOT(manager);
    char* buffer_registros = (char*) malloc(tamanho_buffer);
    fseek(manager->arquivo_dados, offset_bucket + sizeof(BucketHeader), SEEK_SET);
    fread(buffer_registros, 1, tamanho_buffer, manager->arquivo_dados);
    uint32_t nova_profundidade = header.profundidade_local + 1; 
    BucketHeader bucket_antigo = {nova_profundidade, 0};
    BucketHeader bucket_novo = {nova_profundidade, 0};
    fseek(manager->arquivo_dados, 0, SEEK_END);
    long offset_novo_bucket = ftell(manager->arquivo_dados);
    fwrite(&bucket_novo, sizeof(BucketHeader), 1, manager->arquivo_dados);
    void* espaco = calloc(manager->tamanho_bucket, TAMANHO_SLOT(manager));
    fwrite(espaco, TAMANHO_SLOT(manager), manager->tamanho_bucket, manager->arquivo_dados);
    free(espaco);

    uint32_t tamanho_dir = 1 << manager->profundidade_global;
    uint32_t bit_diferenciador = 1 << header.profundidade_local; 

    for (uint32_t i = 0; i < tamanho_dir; i++) {
        if (manager->diretorio[i] == offset_bucket) {
            if ((i & bit_diferenciador) != 0) {
                manager->diretorio[i] = offset_novo_bucket;
            }
        }
    }

    fseek(manager->arquivo_dados, offset_bucket, SEEK_SET);
    fwrite(&bucket_antigo, sizeof(BucketHeader), 1, manager->arquivo_dados);
    for (uint32_t i = 0; i < manager->tamanho_bucket; i++) {
        uint32_t chave_antiga;
        void* dado_antigo = buffer_registros + (i * TAMANHO_SLOT(manager)) + sizeof(uint32_t);
        memcpy(&chave_antiga, buffer_registros + (i * TAMANHO_SLOT(manager)), sizeof(uint32_t));
        
        insert_exHash(hash, chave_antiga, dado_antigo);
    }
    free(buffer_registros);

    return insert_exHash(hash, chave, dado);
}

void* search_exHash(exHash hash, uint32_t chave) {
    ExHashManager* manager = (ExHashManager*) hash;
    if (!manager) return NULL;

    uint32_t indice = obter_indice(chave, manager->profundidade_global);
    long offset_bucket = manager->diretorio[indice];

    BucketHeader header;
    fseek(manager->arquivo_dados, offset_bucket, SEEK_SET);
    fread(&header, sizeof(BucketHeader), 1, manager->arquivo_dados);

    uint32_t chave_lida;
    for (uint32_t i = 0; i < header.quantidade; i++) {
        fread(&chave_lida, sizeof(uint32_t), 1, manager->arquivo_dados);
        
        if (chave_lida == chave) {
            void* dado_encontrado = malloc(manager->tamanho_dado);
            fread(dado_encontrado, manager->tamanho_dado, 1, manager->arquivo_dados);
            return dado_encontrado;
        }
        fseek(manager->arquivo_dados, manager->tamanho_dado, SEEK_CUR);
    }

    return NULL; 
}

bool remove_exHash(exHash hash, uint32_t chave) {
    ExHashManager* manager = (ExHashManager*) hash;
    if (!manager) return false;

    uint32_t indice = obter_indice(chave, manager->profundidade_global);
    long offset_bucket = manager->diretorio[indice];

    BucketHeader header;
    fseek(manager->arquivo_dados, offset_bucket, SEEK_SET);
    fread(&header, sizeof(BucketHeader), 1, manager->arquivo_dados);

    uint32_t chave_lida;
    for (uint32_t i = 0; i < header.quantidade; i++) {
        long posicao_chave = ftell(manager->arquivo_dados);
        fread(&chave_lida, sizeof(uint32_t), 1, manager->arquivo_dados);
        
        if (chave_lida == chave) {
            if (i < header.quantidade - 1) { 
                long posicao_ultimo = offset_bucket + sizeof(BucketHeader) + ((header.quantidade - 1) * TAMANHO_SLOT(manager));
                char* buffer_ultimo = (char*) malloc(TAMANHO_SLOT(manager));
                
                fseek(manager->arquivo_dados, posicao_ultimo, SEEK_SET);
                fread(buffer_ultimo, 1, TAMANHO_SLOT(manager), manager->arquivo_dados);
                
                fseek(manager->arquivo_dados, posicao_chave, SEEK_SET);
                fwrite(buffer_ultimo, 1, TAMANHO_SLOT(manager), manager->arquivo_dados);
                free(buffer_ultimo);
            }

            header.quantidade--;
            fseek(manager->arquivo_dados, offset_bucket, SEEK_SET);
            fwrite(&header, sizeof(BucketHeader), 1, manager->arquivo_dados);
            return true;
        }
        fseek(manager->arquivo_dados, manager->tamanho_dado, SEEK_CUR);
    }

    return false;
}

void close_exHash(exHash hash) {
    ExHashManager* manager = (ExHashManager*) hash;
    if (!manager) return;

    fseek(manager->arquivo_dir, 0, SEEK_SET);
    fwrite(&(manager->tamanho_bucket), sizeof(uint32_t), 1, manager->arquivo_dir);
    fwrite(&(manager->tamanho_dado), sizeof(size_t), 1, manager->arquivo_dir);
    fwrite(&(manager->profundidade_global), sizeof(uint32_t), 1, manager->arquivo_dir);
    
    uint32_t tamanho_dir = 1 << manager->profundidade_global; 
    fwrite(manager->diretorio, sizeof(long), tamanho_dir, manager->arquivo_dir);

    fclose(manager->arquivo_dados);
    fclose(manager->arquivo_dir);

    free(manager->diretorio);
    free(manager);
}
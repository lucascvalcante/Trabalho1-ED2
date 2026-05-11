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
    FILE*    arquivo_dados;
    FILE*    arquivo_dir;
    char     nome_arquivo_base[256];

    uint32_t profundidade_global;
    uint32_t tamanho_bucket;
    size_t   tamanho_dado;
    size_t   tamanho_bucket_bytes;
    size_t   tamanho_registro_completo;

    long*    diretorio;
};

/// --- Funções auxiliares privadas: --- ///

static void* _ler_bucket(exHash h, long offset) {
    void* buf = malloc(h->tamanho_bucket_bytes);
    fseek(h->arquivo_dados, offset, SEEK_SET);
    fread(buf, h->tamanho_bucket_bytes, 1, h->arquivo_dados);
    return buf;
}

static void _gravar_bucket(exHash h, long offset, void* buf) {
    fseek(h->arquivo_dados, offset, SEEK_SET);
    fwrite(buf, h->tamanho_bucket_bytes, 1, h->arquivo_dados);
}

static inline CabecalhoBucket* _cab(void* buf) {
    return (CabecalhoBucket*) buf;
}
static inline Registro* _slot(exHash h, void* buf, uint32_t i) {
    return (Registro*)((char*)buf + sizeof(CabecalhoBucket) + i * h->tamanho_registro_completo);
}
static inline void* _dado_slot(Registro* reg) {
    return (char*)reg + sizeof(Registro);
}

static uint32_t hash_string(const char* str) {
    uint32_t hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

static uint32_t get_bits(uint32_t hash, uint32_t profundidade) {
    if (profundidade == 0) return 0;
    return hash & ((1u << profundidade) - 1);
}

static long _criar_bucket_vazio(exHash h, uint32_t profundidade_local) {
    fseek(h->arquivo_dados, 0, SEEK_END);
    long offset = ftell(h->arquivo_dados);

    void* buf = calloc(1, h->tamanho_bucket_bytes);
    _cab(buf)->profundidade_local = profundidade_local;
    _cab(buf)->qtd_registros      = 0;
    fwrite(buf, h->tamanho_bucket_bytes, 1, h->arquivo_dados);
    free(buf);

    return offset;
}

static bool _insert_interno(exHash h, const char* chave, void* dado) {
    uint32_t idx    = get_bits(hash_string(chave), h->profundidade_global);
    long     offset = h->diretorio[idx];
    void*    buf    = _ler_bucket(h, offset);

    int32_t slot_vazio = -1;
    for (uint32_t i = 0; i < h->tamanho_bucket; i++) {
        Registro* reg = _slot(h, buf, i);
        if (!reg->ocupado && slot_vazio == -1)
            slot_vazio = (int32_t) i;
    }

    if (slot_vazio != -1) {
        Registro* reg = _slot(h, buf, slot_vazio);
        strncpy(reg->chave, chave, MAX_CHAVE - 1);
        reg->chave[MAX_CHAVE - 1] = '\0';
        reg->ocupado = true;
        memcpy(_dado_slot(reg), dado, h->tamanho_dado);
        _cab(buf)->qtd_registros++;
        _gravar_bucket(h, offset, buf);
        free(buf);
        return true;
    }

    CabecalhoBucket cab_snap = *_cab(buf);

    if (cab_snap.profundidade_local == h->profundidade_global) {
        uint32_t tam_ant  = 1u << h->profundidade_global;
        uint32_t tam_novo = 1u << (h->profundidade_global + 1);
        h->diretorio = realloc(h->diretorio, sizeof(long) * tam_novo);
        for (uint32_t i = 0; i < tam_ant; i++)
            h->diretorio[i + tam_ant] = h->diretorio[i];
        h->profundidade_global++;
    }

    void* backup = malloc(h->tamanho_bucket * h->tamanho_registro_completo);
    memcpy(backup, (char*)buf + sizeof(CabecalhoBucket),
           h->tamanho_bucket * h->tamanho_registro_completo);

    memset(buf, 0, h->tamanho_bucket_bytes);
    _cab(buf)->profundidade_local = cab_snap.profundidade_local + 1;
    _gravar_bucket(h, offset, buf);
    free(buf);

    long     novo_offset = _criar_bucket_vazio(h, cab_snap.profundidade_local + 1);
    uint32_t tam_dir     = 1u << h->profundidade_global;
    uint32_t mascara     = 1u << cab_snap.profundidade_local;

    for (uint32_t i = 0; i < tam_dir; i++) {
        if (h->diretorio[i] == offset && (i & mascara) != 0)
            h->diretorio[i] = novo_offset;
    }

    for (uint32_t i = 0; i < h->tamanho_bucket; i++) {
        Registro* reg = (Registro*)((char*)backup + i * h->tamanho_registro_completo);
        if (reg->ocupado)
            _insert_interno(h, reg->chave, _dado_slot(reg));
    }
    free(backup);

    return _insert_interno(h, chave, dado);
}

///--- Funções Públicas: --- ///

exHash init_exHash(const char* arquivo, uint32_t tamanho_bucket, size_t tamanho_dado) {
    exHash h = malloc(sizeof(struct exHash_s));
    if (!h) return NULL;

    strncpy(h->nome_arquivo_base, arquivo, 255);
    h->arquivo_dados = fopen(arquivo, "w+b");

    char nome_dir[300];
    snprintf(nome_dir, sizeof(nome_dir), "%s.dir", arquivo);
    h->arquivo_dir = fopen(nome_dir, "w+b");

    if (!h->arquivo_dados || !h->arquivo_dir) { free(h); return NULL; }

    setvbuf(h->arquivo_dados, NULL, _IOFBF, 64 * 1024);
    setvbuf(h->arquivo_dir,   NULL, _IOFBF, 16 * 1024);

    h->profundidade_global       = 0;
    h->tamanho_bucket            = tamanho_bucket;
    h->tamanho_dado              = tamanho_dado;
    h->tamanho_registro_completo = sizeof(Registro) + tamanho_dado;
    h->tamanho_bucket_bytes      = sizeof(CabecalhoBucket) + tamanho_bucket * h->tamanho_registro_completo;

    h->diretorio    = malloc(sizeof(long));
    h->diretorio[0] = _criar_bucket_vazio(h, 0);

    return h;
}

exHash load_exHash(const char* arquivo) {
    char nome_dir[300];
    snprintf(nome_dir, sizeof(nome_dir), "%s.dir", arquivo);

    FILE* f_dados = fopen(arquivo, "r+b");
    FILE* f_dir   = fopen(nome_dir, "r+b");
    if (!f_dados || !f_dir) {
        if (f_dados) fclose(f_dados);
        if (f_dir)   fclose(f_dir);
        return NULL;
    }

    exHash h = malloc(sizeof(struct exHash_s));
    if (!h) return NULL;

    strncpy(h->nome_arquivo_base, arquivo, 255);
    h->arquivo_dados = f_dados;
    h->arquivo_dir   = f_dir;

    setvbuf(h->arquivo_dados, NULL, _IOFBF, 64 * 1024);
    setvbuf(h->arquivo_dir,   NULL, _IOFBF, 16 * 1024);

    rewind(h->arquivo_dir);
    fread(&h->profundidade_global, sizeof(uint32_t), 1, h->arquivo_dir);
    fread(&h->tamanho_bucket,      sizeof(uint32_t), 1, h->arquivo_dir);
    fread(&h->tamanho_dado,        sizeof(size_t),   1, h->arquivo_dir);

    h->tamanho_registro_completo = sizeof(Registro) + h->tamanho_dado;
    h->tamanho_bucket_bytes      = sizeof(CabecalhoBucket) + h->tamanho_bucket * h->tamanho_registro_completo;

    uint32_t tamanho_dir = 1u << h->profundidade_global;
    h->diretorio = malloc(sizeof(long) * tamanho_dir);
    fread(h->diretorio, sizeof(long), tamanho_dir, h->arquivo_dir);

    return h;
}

void close_exHash(exHash h) {
    if (!h) return;

    rewind(h->arquivo_dir);
    fwrite(&h->profundidade_global, sizeof(uint32_t), 1, h->arquivo_dir);
    fwrite(&h->tamanho_bucket,      sizeof(uint32_t), 1, h->arquivo_dir);
    fwrite(&h->tamanho_dado,        sizeof(size_t),   1, h->arquivo_dir);

    uint32_t tamanho_dir = 1u << h->profundidade_global;
    fwrite(h->diretorio, sizeof(long), tamanho_dir, h->arquivo_dir);

    fclose(h->arquivo_dados);
    fclose(h->arquivo_dir);
    free(h->diretorio);
    free(h);
}

void* search_exHash(exHash h, const char* chave) {
    uint32_t idx = get_bits(hash_string(chave), h->profundidade_global);
    void*    buf = _ler_bucket(h, h->diretorio[idx]);

    for (uint32_t i = 0; i < h->tamanho_bucket; i++) {
        Registro* reg = _slot(h, buf, i);
        if (reg->ocupado && strcmp(reg->chave, chave) == 0) {
            void* dado = malloc(h->tamanho_dado);
            memcpy(dado, _dado_slot(reg), h->tamanho_dado);
            free(buf);
            return dado;
        }
    }

    free(buf);
    return NULL;
}

bool remove_exHash(exHash h, const char* chave) {
    uint32_t idx    = get_bits(hash_string(chave), h->profundidade_global);
    long     offset = h->diretorio[idx];
    void*    buf    = _ler_bucket(h, offset);

    for (uint32_t i = 0; i < h->tamanho_bucket; i++) {
        Registro* reg = _slot(h, buf, i);
        if (reg->ocupado && strcmp(reg->chave, chave) == 0) {
            reg->ocupado = false;
            _cab(buf)->qtd_registros--;
            _gravar_bucket(h, offset, buf);
            free(buf);
            return true;
        }
    }

    free(buf);
    return false;
}


bool update_exHash(exHash h, const char* chave, void* novo_dado) {
    uint32_t idx    = get_bits(hash_string(chave), h->profundidade_global);
    long     offset = h->diretorio[idx];
    void*    buf    = _ler_bucket(h, offset);

    for (uint32_t i = 0; i < h->tamanho_bucket; i++) {
        Registro* reg = _slot(h, buf, i);
        if (reg->ocupado && strcmp(reg->chave, chave) == 0) {
            memcpy(_dado_slot(reg), novo_dado, h->tamanho_dado);
            _gravar_bucket(h, offset, buf);
            free(buf);
            return true;
        }
    }

    free(buf);
    return false;
}

bool insert_exHash(exHash h, const char* chave, void* dado) {
    uint32_t idx    = get_bits(hash_string(chave), h->profundidade_global);
    long     offset = h->diretorio[idx];
    void*    buf    = _ler_bucket(h, offset);

    int32_t slot_vazio = -1;

    for (uint32_t i = 0; i < h->tamanho_bucket; i++) {
        Registro* reg = _slot(h, buf, i);
        if (reg->ocupado) {
            if (strcmp(reg->chave, chave) == 0) {   
                free(buf);
                return false;
            }
        } else if (slot_vazio == -1) {
            slot_vazio = (int32_t) i;
        }
    }

    if (slot_vazio != -1) {
        Registro* reg = _slot(h, buf, slot_vazio);
        strncpy(reg->chave, chave, MAX_CHAVE - 1);
        reg->chave[MAX_CHAVE - 1] = '\0';
        reg->ocupado = true;
        memcpy(_dado_slot(reg), dado, h->tamanho_dado);
        _cab(buf)->qtd_registros++;
        _gravar_bucket(h, offset, buf);
        free(buf);
        return true;
    }

    free(buf);
    return _insert_interno(h, chave, dado);
}

void foreach_exHash(exHash h,
                    void (*func)(const char* chave, void* dado, void* extra),
                    void* extra) {
    if (!h || !func) return;

    long pos_salva = ftell(h->arquivo_dados);
    fseek(h->arquivo_dados, 0, SEEK_SET);

    void* buf = malloc(h->tamanho_bucket_bytes);

    while (fread(buf, h->tamanho_bucket_bytes, 1, h->arquivo_dados) == 1) {
        for (uint32_t i = 0; i < h->tamanho_bucket; i++) {
            Registro* reg = _slot(h, buf, i);
            if (reg->ocupado)
                func(reg->chave, _dado_slot(reg), extra);
        }
    }

    free(buf);
    fseek(h->arquivo_dados, pos_salva, SEEK_SET);
}


void dump_exHash(exHash h, const char* arquivo_saida_hfd) {
    if (!h || !arquivo_saida_hfd) return;
    FILE* saida = fopen(arquivo_saida_hfd, "w");
    if (!saida) return;

    fprintf(saida, "=== ESTRUTURA DO HASH EXTENSIVEL ===\n");
    fprintf(saida, "Profundidade Global: %u\n", h->profundidade_global);
    fprintf(saida, "Tamanho maximo do Bucket: %u registros\n\n", h->tamanho_bucket);

    uint32_t tamanho_dir = 1u << h->profundidade_global;
    fprintf(saida, "--- DIRETORIO (Tamanho: %u) ---\n", tamanho_dir);
    for (uint32_t i = 0; i < tamanho_dir; i++)
        fprintf(saida, "Indice [%03u] -> Offset: %ld\n", i, h->diretorio[i]);

    fprintf(saida, "\n--- CONTEUDO DOS BUCKETS NO DISCO ---\n");

    long pos_salva = ftell(h->arquivo_dados);
    fseek(h->arquivo_dados, 0, SEEK_SET);

    void* buf = malloc(h->tamanho_bucket_bytes);
    int   bucket_id = 0;

    while (1) {
        long cur = ftell(h->arquivo_dados);
        if (fread(buf, h->tamanho_bucket_bytes, 1, h->arquivo_dados) != 1) break;

        fprintf(saida, "\n[Bucket %d] - Offset: %ld\n", bucket_id++, cur);
        fprintf(saida, "  Profundidade Local: %u\n", _cab(buf)->profundidade_local);
        fprintf(saida, "  Registros Ocupados: %u / %u\n",
                _cab(buf)->qtd_registros, h->tamanho_bucket);

        for (uint32_t i = 0; i < h->tamanho_bucket; i++) {
            Registro* reg = _slot(h, buf, i);
            if (reg->ocupado)
                fprintf(saida, "    [%u] -> Ocupado | Chave: %s\n", i, reg->chave);
            else
                fprintf(saida, "    [%u] -> [Vazio]\n", i);
        }
    }

    free(buf);
    fseek(h->arquivo_dados, pos_salva, SEEK_SET);
    fprintf(saida, "\n====================================\n");
    fclose(saida);
}
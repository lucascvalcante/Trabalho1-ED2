/**
 * @file exHash.h
 * @brief Módulo de Hashing Dinâmico (Extensível) em Disco.
 * * Este módulo implementa uma estrutura de tabela hash extensível armazenada
 * em arquivo binário (.hf). Ele é responsável por gerenciar a inserção, 
 * busca e remoção de registros em disco utilizando chaves alfanuméricas 
 * (como CEPs e CPFs). O módulo também gerencia o particionamento (split) de 
 * buckets quando a capacidade máxima é atingida e gera relatórios estruturais (.hfd).
 */

#ifndef EXHASH_H
#define EXHASH_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Tipo opaco usando forward declaration. 
// Garante o encapsulamento e segurança de tipo.
typedef struct exHash_s* exHash;


/// @brief Cria um novo arquivo de hashing extensível
/// @param arquivo Caminho do arquivo
/// @param tamanho_bucket Quantidade máxima de registros que cabem em um bucket
/// @param tamanho_dado Tamanho do dado que será salvo 
/// @return Um ponteiro para o hash inicializado, ou NULL em caso de erro
exHash init_exHash(const char* arquivo, uint32_t tamanho_bucket, size_t tamanho_dado);


/// @brief Carrega um arquivo de Hashing Extensível que já existe no disco
/// @param arquivo Caminho do arquivo de origem
/// @return Um ponteiro para a estrutura carregada com os dados do disco, ou NULL
exHash load_exHash(const char* arquivo);


/// @brief Insere um novo par chave-valor no disco. Se o bucket encher, faz o split
/// @param hash Ponteiro para a estrutura do hashing
/// @param chave Chave alfanumérica (CEP ou CPF) usada para indexar e tratar colisões
/// @param dado Ponteiro genérico para o dado que será armazenado
/// @return 'True' se a inserção ocorreu com sucesso, 'false' em caso de erro/chave duplicada.
bool insert_exHash(exHash hash, const char* chave, void* dado);

/// @brief Busca um dado no disco através da sua chave.
/// @param hash Ponteiro para a estrutura do hashing
/// @param chave Chave de busca (CEP ou CPF)
/// @return Ponteiro para uma cópia do dado encontrado na memória ram
void* search_exHash(exHash hash, const char* chave);


/// @brief Remove um registro do arquivo
/// @param hash Ponteiro para a estrutura do hashing
/// @param chave Chave a ser removida (CEP ou CPF)
/// @return 'True' se o registro foi encontrado e removido, 'false' caso contrário
bool remove_exHash(exHash hash, const char* chave);


/// @brief Produz um arquivo-texto com a representação esquemática do hashfile e expansões
/// @param hash Ponteiro para a estrutura do hashing
/// @param arquivo_saida_hfd Caminho para o arquivo .hfd que será gerado
void dump_exHash(exHash hash, const char* arquivo_saida_hfd);


/// @brief Fecha os arquivos em disco, salva o estado do diretório e libera a memória ram
/// @param hash Ponteiro para a estrutura do hashing
void close_exHash(exHash hash);

#endif
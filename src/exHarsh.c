#ifndef EXHASH_H
#define EXHASH_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


typedef void* exHash;



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
/// @param chave Chave numérica usada para calcular o hash e indexar
/// @param dado Ponteiro genérico para o dado que será armazenado
/// @return 'True' se a inserção ocorreu com sucesso, 'false' em caso de erro/chave duplicada.
bool insert_exHash(exHash hash, uint32_t chave, void* dado);


/// @brief Busca um dado no disco através da sua chave.
/// @param hash Ponteiro para a estrutura do hashing
/// @param chave Chave de busca
/// @return Ponteiro para uma cópia do dado encontrado na memória ram
void* search_exHash(exHash hash, uint32_t chave);


/// @brief Remove um registro do arquivo
/// @param hash Ponteiro para a estrutura do hashing
/// @param chave Chave a ser removida
/// @return 'True' se o registro foi encontrado e removido, 'false' caso contrário
bool remove_exHash(exHash hash, uint32_t chave);


/// @brief Fecha os arquivos em disco, salva o estado do diretório e libera a memória ram
/// @param hash Ponteiro para a estrutura do hashing
void close_exHash(exHash hash);

#endif 
#ifndef GEO_H
#define GEO_H

#include "exHash.h"

/**
 * @file geo.h
 * @brief Módulo de Processamento de Geometria Urbana (.geo).
 *
 * Este módulo é responsável por realizar o parsing (leitura e interpretação) 
 * de arquivos de geometria urbana. Ele identifica os comandos de criação 
 * de quadras e popula a estrutura de dados persistente (exHash), garantindo 
 * que as informações espaciais da cidade sejam devidamente armazenadas em disco.
 */
 

/// @brief Abre o arquivo .geo, lê os comandos linha por linha e popula o Hash de quadras
/// @param caminho_arquivo Caminho para o arquivo .geo 
/// @param hash_quadras Ponteiro para o Hash Extensível onde as quadras serão salvas
void processa_arquivo_geo(const char* caminho_arquivo, exHash hash_quadras);

#endif 
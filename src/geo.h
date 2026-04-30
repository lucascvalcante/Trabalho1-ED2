#ifndef GEO_H
#define GEO_H

#include "exHash.h"

/// @brief Abre o arquivo .geo, lê os comandos linha por linha e popula o Hash de quadras
/// @param caminho_arquivo Caminho para o arquivo .geo 
/// @param hash_quadras Ponteiro para o Hash Extensível onde as quadras serão salvas
void processa_arquivo_geo(const char* caminho_arquivo, exHash hash_quadras);

#endif 
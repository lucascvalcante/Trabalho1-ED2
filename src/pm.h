#ifndef PM_H
#define PM_H

#include "exHash.h"

/* Responsável por interpretar os arquivos de texto de entrada e executar
 * os comandos correspondentes no banco de dados.*/

/// @brief Abre o arquivo .pm, lê os comandos linha por linha e popula o Hash.
/// @param caminho_arquivo Caminho para o arquivo .pm 
/// @param hash_hab Ponteiro para o Hash Extensível onde os habitantes serão salvos
void processa_arquivo_pm(const char* caminho_arquivo, exHash hash_hab);

#endif
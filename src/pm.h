#ifndef PM_H
#define PM_H

#include "exHash.h"

/**
 * @file pm.h
 * @brief Módulo de Processamento de Pessoas e Moradias (.pm).
 *
 * Este módulo gerencia a leitura de arquivos de dados populacionais. 
 * É responsável por cadastrar novos habitantes e processar mudanças de 
 * endereço, realizando a integração entre os moradores e as quadras 
 * armazenadas no sistema, utilizando operações otimizadas de busca e 
 * atualização em disco.
 */

/// @brief Abre o arquivo .pm, lê os comandos linha por linha e popula o Hash.
/// @param caminho_arquivo Caminho para o arquivo .pm 
/// @param hash_hab Ponteiro para o Hash Extensível onde os habitantes serão salvos
void processa_arquivo_pm(const char* caminho_arquivo, exHash hash_hab);

#endif
#ifndef QRY_H
#define QRY_H

#include "exHash.h"

/// @brief Lê um arquivo .qry, executa as ações e gera arquivos txt e svg de saída
/// @param caminho_qry Arquivo de entrada .qry
/// @param caminho_txt Arquivo de saída .txt
/// @param caminho_svg Arquivo de saída .svg 
/// @param h_hab Hash de habitantes
/// @param h_quadras Hash de quadras
void processa_arquivo_qry(const char* caminho_qry, const char* caminho_txt, const char* caminho_svg, exHash h_hab, exHash h_quadras);

#endif
#ifndef SVG_H
#define SVG_H

#include <stdio.h>
#include "exHash.h"
#include "quadra.h"

/**
 * @file svg.h
 * @brief Módulo de Geração e Manipulação de Imagens Vetoriais (SVG).
 *
 * Este módulo é responsável pela criação e escrita de arquivos gráficos no 
 * formato SVG. Ele fornece funções para desenhar formas geométricas (retângulos, 
 * círculos, cruzes) e textos, permitindo a visualização espacial da cidade 
 * (renderização das quadras) e a marcação visual dos resultados das consultas 
 * realizadas no sistema.
 */

 
/// @brief Inicia o arquivo SVG escrevendo o cabecalho padrao
/// @param caminho Caminho para o arquivo .svg a ser criado
/// @return Ponteiro para o arquivo 
FILE* abre_svg(const char* caminho);


/// @brief Escreve a tag de fechamento e fecha o arquivo.
/// @param f Ponteiro para o arquivo SVG
void fecha_svg(FILE* f);


/// @brief Desenha um retangulo representando uma quadra
/// @param f Ponteiro para o arquivo SVG
/// @param x Coordenada X (canto superior esquerdo)
/// @param y Coordenada Y (canto superior esquerdo)
/// @param w Largura da quadra
/// @param h Altura da quadra
/// @param fill Cor de preenchimento
/// @param strk Cor da borda
/// @param sw Espessura da borda
void svg_quadra(FILE* f, double x, double y, double w, double h, const char* fill, const char* strk, const char* sw);


/// @brief Desenha um X vermelho 
/// @param f Ponteiro para o arquivo SVG
/// @param x Coordenada X do centro do X
/// @param y Coordenada Y do centro do X
void svg_marca_x(FILE* f, double x, double y);


/// @brief Desenha uma cruz vermelha 
/// @param f Ponteiro para o arquivo SVG
/// @param x Coordenada X do centro da cruz
/// @param y Coordenada Y do centro da cruz
void svg_marca_cruz(FILE* f, double x, double y);


/// @brief Desenha um pequeno circulo preto 
/// @param f Ponteiro para o arquivo SVG
/// @param x Coordenada X do centro do circulo
/// @param y Coordenada Y do centro do circulo
void svg_marca_circulo(FILE* f, double x, double y);


/// @brief Escreve um texto no SVG (usado nos comandos pq e mud).
/// @param f Ponteiro para o arquivo SVG
/// @param x Coordenada X do texto
/// @param y Coordenada Y do texto
/// @param txt Conteudo do texto
/// @param cor Cor do texto
/// @param anchor Alinhamento do texto ("start", "middle" ou "end")
void svg_texto(FILE* f, double x, double y, const char* txt, const char* cor, const char* anchor);


/// @brief Gera um svg com os dados do arquivo .geo
/// @param caminho_svg path do svg
/// @param hash_quadras hash file contendo as quadras que serão desenhadas
void gerar_svg_geo(const char* caminho_svg, exHash hash_quadras);


/// @brief Função de callback para desenhar uma quadra (retângulo) e escrever seu CEP no arquivo SVG
/// @param chave Chave do elemento atual na tabela hash (O CEP da quadra)
/// @param registro Ponteiro genérico para o dado armazenado na hash (A estrutura da Quadra)
/// @param arquivo Ponteiro genérico contendo o arquivo de saída (O FILE* do SVG aberto)
void escreve_retangulo_svg(const char* chave, void* registro, void* arquivo);

#endif

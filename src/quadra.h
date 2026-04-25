#ifndef QUADRA_H
#define QUADRA_H

typedef struct quadra_s* Quadra;

// @brief Cria e aloca dinamicamente uma nova quadra.
// @param cep Identificador alfanumerico da quadra
// @param x Coordenada X do canto superior esquerdo
// @param y Coordenada Y do canto superior esquerdo
// @param w Largura da quadra
// @param h Altura da quadra
// @param sw Espessura da borda
// @param cfill Cor de preenchimento
// @param cstrk Cor da borda
// @return Ponteiro para a quadra criada
Quadra cria_quadra(const char* cep, double x, double y, double w, double h, const char* sw, const char* cfill, const char* cstrk);

// @brief Retorna o cep da quadra
// @param q Ponteiro para a quadra
// @return O cep
const char* get_quadra_cep(Quadra q);

// @brief Retorna a coordenada X do canto superior esquerdo da quadra
// @param q Ponteiro para a quadra
// @return A coordenada X
double get_quadra_x(Quadra q);

// @brief Retorna a coordenada Y do canto superior esquerdo da quadra
// @param q Ponteiro para a quadra
// @return A coordenada Y
double get_quadra_y(Quadra q);

// @brief Retorna a largura da quadra
// @param q Ponteiro para a quadra
// @return A largura
double get_quadra_w(Quadra q);

// @brief Retorna a altura da quadra
// @param q Ponteiro para a quadra
// @return A altura
double get_quadra_h(Quadra q);

// @brief Retorna a cor de preenchimento da quadra
// @param q Ponteiro para a quadra
// @return A cor de preenchimento
const char* get_quadra_cfill(Quadra q);

// @brief Retorna a cor da borda da quadra
// @param q Ponteiro para a quadra
// @return A cor da borda
const char* get_quadra_cstrk(Quadra q);

// @brief Retorna a espessura da borda da quadra
// @param q Ponteiro para a quadra
// @return A espessura da borda
const char* get_quadra_sw(Quadra q);

// @brief Calcula as coordenadas do ponto de ancoragem (canto sudeste) da quadra.
// @param q Ponteiro para a quadra
// @param ancora_x Ponteiro onde sera salvo o valor X da ancora
// @param ancora_y Ponteiro onde sera salvo o valor Y da ancora
void get_quadra_ancora(Quadra q, double* ancora_x, double* ancora_y);

// @brief Libera a memoria alocada para a quadra.
// @param q Ponteiro para a quadra
void free_quadra(Quadra q);

#endif
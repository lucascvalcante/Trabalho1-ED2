#ifndef HABITANTE_H
#define HABITANTE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef struct habitante_s* Habitante;

/// @brief Obtém o tamanho exato da estrutura do habitante em memória
/// @return O tamanho da estrutura em bytes
size_t get_habitante_size();

/// @brief Cria e inicializa um novo habitante alocado dinamicamente 
/// @param cpf String contendo o CPF do habitante 
/// @param nome String contendo o primeiro nome
/// @param sobrenome String contendo o sobrenome
/// @param sexo Caractere representando o gênero do habitante 
/// @param nascimento String contendo a data de nascimento
/// @return Ponteiro para o Habitante criado, ou NULL em caso de falha de memória.
Habitante cria_habitante(const char* cpf, const char* nome, const char* sobrenome, char sexo, const char* nascimento);


/// @brief Libera toda a memória alocada para o habitante
/// @param h Ponteiro para o habitante que será destruído
void free_habitante(Habitante h);

/// @brief Atualiza todos os dados de endereço e remove o status de sem-teto
/// @param h Ponteiro para o habitante
/// @param cep String com o CEP da quadra onde a casa está localizada
/// @param face Caractere indicando a face da quadra 
/// @param numero Distância ou número da casa na referida face
/// @param complemento String com o complemento 
void set_habitante_endereco(Habitante h, const char* cep, char face, double numero, const char* complemento);


/// @brief Limpa os dados de endereço do habitante
/// @param h Ponteiro para o habitante
void set_habitante_sem_teto(Habitante h);


/// @brief Verifica o status de moradia do habitante
/// @param h Ponteiro para o habitante
/// @return Retorna true se for sem-teto, ou false caso possua um endereço cadastrada
bool is_sem_teto(Habitante h);


/// @brief Obtém o CPF do habitante
/// @param h Ponteiro para o habitante
/// @return String inalterável (const char*) com o CPF, ou NULL se h for inválido
const char* get_habitante_cpf(Habitante h);


/// @brief Obtém o nome do habitante
/// @param h Ponteiro para o habitante
/// @return String inalterável (const char*) com o nome, ou NULL se h for inválido
const char* get_habitante_nome(Habitante h);

/// @brief Obtém o sobrenome do habitante
/// @param h Ponteiro para o habitante
/// @return String inalterável (const char*) com o sobrenome, ou NULL se h for inválido
const char* get_habitante_sobrenome(Habitante h);

/// @brief Obtém o sexo do habitante
/// @param h Ponteiro para o habitante
/// @return Caractere de sexo, ou '\0' se h for inválido
char get_habitante_sexo(Habitante h);


/// @brief Obtém a data de nascimento do habitante
/// @param h Ponteiro para o habitante
/// @return String inalterável (const char*) com a data, ou NULL se h for inválido
const char* get_habitante_nascimento(Habitante h);


/// @brief Obtém o CEP da residência atual do habitante
/// @param h Ponteiro para o habitante
/// @return String inalterável com o CEP, ou NULL se for sem-teto/inválido.
const char* get_habitante_cep(Habitante h);


/// @brief Obtém a face da quadra onde o habitante mora
/// @param h Ponteiro para o habitante
/// @return Caractere da face, ou '\0' se for sem-teto/inválido
char get_habitante_face(Habitante h);


/// @brief Obtém o número/distância da casa do habitante
/// @param h Ponteiro para o habitante
/// @return Valor numérico (double), ou 0.0 se for sem-teto/inválido
double get_habitante_numero(Habitante h);


/// @brief Obtém o complemento do endereço do habitante
/// @param h Ponteiro para o habitante
/// @return String inalterável com o complemento, ou NULL se for sem-teto/inválido
const char* get_habitante_complemento(Habitante h);


/// @brief Formata e escreve os dados completos do habitante em um arquivo
/// @param txt Ponteiro para o arquivo já aberto para escrita
/// @param h Ponteiro para o habitante a ser impresso
void print_habitante_info(FILE* txt, Habitante h);

#endif
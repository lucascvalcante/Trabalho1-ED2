#include "pm.h"
#include "habitante.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void processa_arquivo_pm(const char* caminho_arquivo, exHash hash_hab) {
    if (caminho_arquivo == NULL || hash_hab == NULL) {
        printf("Erro: Arquivo ou Hash inválidos.\n");
        return;
    }

    FILE* arq = fopen(caminho_arquivo, "r");
    if (arq == NULL) {
        printf("Erro: Nao foi possivel abrir o arquivo '%s'.\n", caminho_arquivo);
        return;
    }

    char linha[256]; 

    while (fgets(linha, sizeof(linha), arq) != NULL) {
        linha[strcspn(linha, "\r\n")] = 0; 
        if (strlen(linha) == 0) continue;

        char comando = linha[0];

        if (comando == 'p') {
            char cpf[15], nome[50], sobrenome[50], nasc[11];
            char sexo;
            sscanf(linha, "p %14s %49s %49s %c %10s", cpf, nome, sobrenome, &sexo, nasc);

            Habitante novo_hab = cria_habitante(cpf, nome, sobrenome, sexo, nasc);
            if (novo_hab) {
                insert_exHash(hash_hab, cpf, novo_hab);
                free_habitante(novo_hab); 
            }
        } 
        else if (comando == 'm') {
            char cpf[15], cep[20], compl[50] = "";
            char face;
            double num;

            sscanf(linha, "m %14s %19s %c %lf %49[^\n]", cpf, cep, &face, &num, compl);

            Habitante morador = (Habitante) search_exHash(hash_hab, cpf);
            
            if (morador != NULL) {
                set_habitante_endereco(morador, cep, face, num, compl);                
                remove_exHash(hash_hab, cpf);
                insert_exHash(hash_hab, cpf, morador);                 
                free_habitante(morador);
            } else {
                printf("Aviso: Tentativa de dar endereço a um CPF não cadastrado (%s).\n", cpf);
            }
        }
    }

    fclose(arq);
    printf("Arquivo '%s' processado com sucesso!\n", caminho_arquivo);
}
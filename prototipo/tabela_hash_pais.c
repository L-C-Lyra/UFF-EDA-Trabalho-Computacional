#include "tabela_hash_pais.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

TabelaHashPais* criar_tabela_hash_pais() {
    TabelaHashPais *cht = (TabelaHashPais*) malloc(sizeof(TabelaHashPais));
    if (cht == NULL) {
        perror("Erro ao alocar TabelaHashPais");
        return NULL;
    }
    for (int i = 0; i < COUNTRY_HASH_TABLE_SIZE; i++) {
        cht->list_heads[i] = NULL;
    }
    return cht;
}

unsigned int hash_string_pais(const char *str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % COUNTRY_HASH_TABLE_SIZE;
}

int inserir_tabela_hash_pais(TabelaHashPais *cht, const char *nacionalidade, const char *nome_completo, int id_folha, int indice_registro_na_folha) {
    if (cht == NULL || nacionalidade == NULL || nome_completo == NULL) {
        return 0;
    }

    unsigned int index = hash_string_pais(nacionalidade);

    EntradaHashPais *country_entry = cht->list_heads[index];
    while (country_entry != NULL && strcmp(country_entry->nacionalidade, nacionalidade) != 0) {
        country_entry = country_entry->proximo;
    }

    if (country_entry == NULL) {
        country_entry = (EntradaHashPais*) malloc(sizeof(EntradaHashPais));
        if (country_entry == NULL) {
            perror("Erro ao alocar EntradaHashPais");
            return 0;
        }
        strncpy(country_entry->nacionalidade, nacionalidade, sizeof(country_entry->nacionalidade) - 1);
        country_entry->nacionalidade[sizeof(country_entry->nacionalidade) - 1] = '\0';
        country_entry->lista_jogadores_cabeca = NULL;
        country_entry->proximo = cht->list_heads[index];
        cht->list_heads[index] = country_entry;
    }

    NoLocalizacaoJogador *new_player_node = (NoLocalizacaoJogador*) malloc(sizeof(NoLocalizacaoJogador));
    if (new_player_node == NULL) {
        perror("Erro ao alocar NoLocalizacaoJogador");
        return 0;
    }
    strncpy(new_player_node->nome_completo, nome_completo, sizeof(new_player_node->nome_completo) - 1); // copia o nome completo do jogador para o novo no

    new_player_node->nome_completo[sizeof(new_player_node->nome_completo) - 1] = '\0';

    new_player_node->id_folha = id_folha;
    new_player_node->indice_registro_na_folha = indice_registro_na_folha; // armazena a localização do registro do jogador na arv B+.
    new_player_node->proximo = NULL; // prox como null

    new_player_node->proximo = country_entry->lista_jogadores_cabeca;
    country_entry->lista_jogadores_cabeca = new_player_node;

    return 1;
}

NoLocalizacaoJogador* buscar_tabela_hash_pais(TabelaHashPais *cht, const char *nacionalidade) {
    if (cht == NULL || nacionalidade == NULL) {
        return NULL;
    }

    unsigned int index = hash_string_pais(nacionalidade);
    EntradaHashPais *country_entry = cht->list_heads[index];

    while (country_entry != NULL) {
        if (strcmp(country_entry->nacionalidade, nacionalidade) == 0) {
            return country_entry->lista_jogadores_cabeca;
        }
        country_entry = country_entry->proximo;
    }

    return NULL;
}

void deletar_tabela_hash_pais(TabelaHashPais* cht, const char* nacionalidade, const char* full_name) {
    if (!cht || !nacionalidade || !full_name) return;

    unsigned int index = hash_string_pais(nacionalidade);

    EntradaHashPais* country_entry = cht->list_heads[index];
    while (country_entry != NULL && strcmp(country_entry->nacionalidade, nacionalidade) != 0) {
        country_entry = country_entry->proximo;
    }

    if (country_entry == NULL) return;

    NoLocalizacaoJogador* current_player = country_entry->lista_jogadores_cabeca;
    NoLocalizacaoJogador* prev_player = NULL;

    while (current_player != NULL) {
        if (strcmp(current_player->nome_completo, full_name) == 0) {
            if (prev_player == NULL) {
                country_entry->lista_jogadores_cabeca = current_player->proximo;
            }
            else {
                prev_player->proximo = current_player->proximo;
            }
            free(current_player);
            return;
        }
        prev_player = current_player;
        current_player = current_player->proximo;
    }
}

void liberar_tabela_hash_pais(TabelaHashPais *cht) {
    if (cht == NULL) {
        return;
    }
    for (int i = 0; i < COUNTRY_HASH_TABLE_SIZE; i++) {
        EntradaHashPais *current_country_entry = cht->list_heads[i];
        while (current_country_entry != NULL) {
            NoLocalizacaoJogador *current_player_node = current_country_entry->lista_jogadores_cabeca;
            while (current_player_node != NULL) {
                NoLocalizacaoJogador *temp_player = current_player_node;
                current_player_node = current_player_node->proximo;
                free(temp_player);
            }

            EntradaHashPais *temp_country = current_country_entry;
            current_country_entry = current_country_entry->proximo;
            free(temp_country);
        }
        cht->list_heads[i] = NULL;
    }
    free(cht);
}
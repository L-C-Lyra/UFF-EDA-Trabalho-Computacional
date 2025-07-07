#include "operations.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void search_player_by_name(HashTable* player_ht, const char* full_name) {
    int leaf_id, record_idx;
    printf("\nBuscando por '%s'...\n", full_name);

    if (hash_table_search(player_ht, full_name, &leaf_id, &record_idx)) {
        LeafNode* leaf = read_leaf_node(leaf_id);
        if (leaf) {
            print_player(&leaf->records[record_idx]);
            free(leaf);
        }
    } else {
        printf("-> Jogador '%s' NAO ENCONTRADO.\n", full_name);
    }
}

void search_players_by_country(TabelaHashPais* country_ht, const char* country_name) {
    printf("\nBuscando jogadores de '%s'...\n", country_name);
    NoLocalizacaoJogador* player_node = buscar_tabela_hash_pais(country_ht, country_name);

    if (player_node == NULL) {
        printf("-> Nenhum jogador encontrado para o pais '%s'.\n", country_name);
        return;
    }

    int count = 0;
    while(player_node != NULL) {
        LeafNode* leaf = read_leaf_node(player_node->id_folha);
        if (leaf) {
            print_player(&leaf->records[player_node->indice_registro_na_folha]);
            count++;
            free(leaf);
        }
        player_node = player_node->proximo;
    }
    printf("-> Total de %d jogadores encontrados.\n", count);
}

void delete_player_by_name(FILE* index_file, HashTable* player_ht, TabelaHashPais* country_ht, const char* full_name) {
    int leaf_id, record_idx;

    if (!hash_table_search(player_ht, full_name, &leaf_id, &record_idx)) {
        printf("-> Jogador '%s' nao encontrado para remocao.\n", full_name);
        return;
    }

    LeafNode* leaf = read_leaf_node(leaf_id);
    if (!leaf) {
        printf("-> Erro critico: nao foi possivel ler a folha do jogador.\n");
        return;
    }

    PlayerData player_to_delete = leaf->records[record_idx];
    free(leaf);

    printf("  - Removendo '%s' (Ano: %d) da Arvore B+...\n", full_name, player_to_delete.birth_year);
    bpt_delete(index_file, player_to_delete.birth_year, full_name);

    printf("  - Removendo '%s' da tabela hash de nomes...\n", full_name);
    hash_table_delete(player_ht, full_name);

    printf("  - Removendo '%s' da tabela hash de paises (%s)...\n", full_name, player_to_delete.nacionality);
    deletar_tabela_hash_pais(country_ht, player_to_delete.nacionality, full_name);

    printf("  - '%s' removido de todas as estruturas.\n", full_name);
}

void delete_players_by_country(FILE* index_file, HashTable* player_ht, TabelaHashPais* country_ht, const char* country_name) {
    printf("\nIniciando remocao de todos os jogadores de '%s'...\n", country_name);
    NoLocalizacaoJogador* player_list = buscar_tabela_hash_pais(country_ht, country_name);

    if (player_list == NULL) {
        printf("-> Nenhum jogador encontrado para o pais '%s'.\n", country_name);
        return;
    }

    char names_to_delete[300][NAME_SIZE * 2];
    int count = 0;
    NoLocalizacaoJogador* current = player_list;
    while(current != NULL && count < 300) {
        strcpy(names_to_delete[count++], current->nome_completo);
        current = current->proximo;
    }

    for (int i = 0; i < count; i++) {
        delete_player_by_name(index_file, player_ht, country_ht, names_to_delete[i]);
    }

    printf("-> Operacao concluida. %d jogador(es) de '%s' foram removidos.\n", count, country_name);
    printf("-> AVISO: As tabelas hash devem ser recarregadas para consistencia total.\n");
}
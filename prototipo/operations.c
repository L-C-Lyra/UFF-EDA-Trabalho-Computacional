#include "operations.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int year;
    char winner_lastname[NAME_SIZE];
    char winner_nacionality[NAME_SIZE];
} GrandSlamWinner;

void search_player_by_name(HashTable* player_ht, const char* full_name) {
    int leaf_id, record_idx;
    printf("\nBuscando por '%s'...\n", full_name);

    if (hash_table_search(player_ht, full_name, &leaf_id, &record_idx)) {
        LeafNode* leaf = read_leaf_node(leaf_id);
        if (leaf) {
            PlayerData* p = &leaf->records[record_idx];
            // Confirma o nome completo antes de imprimir
            char current_full_name[FULL_NAME_SIZE];
            sprintf(current_full_name, "%s %s", p->name, p->lastname);
            if(strcmp(full_name, current_full_name) == 0){
                print_player(p);
            } else {
                 printf("-> Erro de consistencia na Hash. Nome encontrado: '%s'\n", current_full_name);
            }
            free(leaf);
        }
    } else {
        printf("-> Jogador '%s' NAO ENCONTRADO.\n", full_name);
    }
}

void search_players_by_country(FILE* index_file, TabelaHashPais* country_ht, const char* country_name) {
    printf("\nBuscando jogadores de '%s'...\n", country_name);

    NoLocalizacaoJogador* player_node = buscar_tabela_hash_pais(country_ht, country_name);

    if (player_node == NULL) {
        printf("-> Nenhum jogador encontrado para o pais '%s'.\n", country_name);
        return;
    }

    int count = 0;
    while(player_node != NULL) {
        PlayerData* found_player = bpt_search(index_file, player_node->nome_completo);

        if (found_player != NULL) {
            print_player(found_player);
            free(found_player);
            count++;
        } else {
            printf("-> ERRO DE CONSISTÊNCIA: Jogador '%s' encontrado na hash de país, mas não na Árvore B+.\n", player_node->nome_completo);
        }

        player_node = player_node->proximo;
    }
    printf("-> Total de %d jogadores encontrados e validados na Arvore B+.\n", count);
}

void search_active_player_by_name(HashTable* player_ht, const char* full_name) {
    printf("\nBuscando jogador EM ATIVIDADE por nome: '%s'...\n", full_name);
    int leaf_id, record_idx;

    if (hash_table_search(player_ht, full_name, &leaf_id, &record_idx)) {
        LeafNode* leaf = read_leaf_node(leaf_id);
        if (leaf) {
            PlayerData* p = &leaf->records[record_idx];
            if (p->is_retired == 0) {
                print_player(p);
            } else {
                printf("-> Jogador '%s' encontrado, mas esta APOSENTADO.\n", full_name);
            }
            free(leaf);
        }
    } else {
        printf("-> Jogador '%s' NAO ENCONTRADO.\n", full_name);
    }
}

void search_active_players_by_country(FILE* index_file, TabelaHashPais* country_ht, const char* country_name) {
    printf("\nBuscando jogadores EM ATIVIDADE de '%s'...\n", country_name);

    NoLocalizacaoJogador* player_node = buscar_tabela_hash_pais(country_ht, country_name);

    if (player_node == NULL) {
        printf("-> Nenhum jogador encontrado para o pais '%s'.\n", country_name);
        return;
    }

    int count = 0;
    while(player_node != NULL) {
        PlayerData* found_player = bpt_search(index_file, player_node->nome_completo);
        if (found_player != NULL) {
            if (found_player->is_retired == 0) {
                print_player(found_player);
                count++;
            }
            free(found_player);
        }
        player_node = player_node->proximo;
    }
    printf("-> Total de %d jogadores em atividade encontrados para '%s'.\n", count, country_name);
}

void search_retired_player_by_name(HashTable* player_ht, const char* full_name) {
    printf("\nBuscando jogador APOSENTADO por nome: '%s'...\n", full_name);
    int leaf_id, record_idx;

    if (hash_table_search(player_ht, full_name, &leaf_id, &record_idx)) {
        LeafNode* leaf = read_leaf_node(leaf_id);
        if (leaf) {
            PlayerData* p = &leaf->records[record_idx];
            if (p->is_retired == 1) {
                print_player(p);
            } else {
                printf("-> Jogador '%s' encontrado, mas esta EM ATIVIDADE.\n", full_name);
            }
            free(leaf);
        }
    } else {
        printf("-> Jogador '%s' NAO ENCONTRADO.\n", full_name);
    }
}

void search_retired_players_by_country(FILE* index_file, TabelaHashPais* country_ht, const char* country_name) {
    printf("\nBuscando jogadores APOSENTADOS de '%s'...\n", country_name);
    NoLocalizacaoJogador* player_node = buscar_tabela_hash_pais(country_ht, country_name);

    if (player_node == NULL) {
        printf("-> Nenhum jogador encontrado para o pais '%s'.\n", country_name);
        return;
    }

    int count = 0;
    while(player_node != NULL) {
        PlayerData* found_player = bpt_search(index_file, player_node->nome_completo);
        if (found_player != NULL) {
            if (found_player->is_retired == 1) {
                print_player(found_player);
                count++;
            }
            free(found_player);
        }
        player_node = player_node->proximo;
    }
    printf("-> Total de %d jogadores aposentados encontrados para '%s'.\n", count, country_name);
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

    // CORRIGIDO: chamada da bpt_delete
    printf("  - Removendo '%s' da Arvore B+...\n", full_name);
    bpt_delete(index_file, full_name);

    printf("  - Removendo '%s' da tabela hash de nomes...\n", full_name);
    hash_table_delete(player_ht, full_name);

    printf("  - Removendo '%s' da tabela hash de paises (%s)...\n", full_name, player_to_delete.nacionality);
    deletar_tabela_hash_pais(country_ht, player_to_delete.nacionality, full_name);

    printf("-> '%s' removido de todas as estruturas.\n", full_name);
}

void delete_players_by_country(FILE* index_file, HashTable* player_ht, TabelaHashPais* country_ht, const char* country_name) {
    printf("\nIniciando remocao de todos os jogadores de '%s'...\n", country_name);
    NoLocalizacaoJogador* player_list = buscar_tabela_hash_pais(country_ht, country_name);

    if (player_list == NULL) {
        printf("-> Nenhum jogador encontrado para o pais '%s'.\n", country_name);
        return;
    }

    char names_to_delete[300][FULL_NAME_SIZE];
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
}

void delete_active_players_by_country(FILE* index_file, HashTable* player_ht, TabelaHashPais* country_ht, const char* country_name) {
    printf("\nIniciando remocao de jogadores EM ATIVIDADE de '%s'...\n", country_name);
    NoLocalizacaoJogador* player_list = buscar_tabela_hash_pais(country_ht, country_name);

    if (player_list == NULL) {
        printf("-> Nenhum jogador encontrado para o pais '%s'.\n", country_name);
        return;
    }

    char names_to_delete[300][FULL_NAME_SIZE];
    int count = 0;
    NoLocalizacaoJogador* current = player_list;

    while(current != NULL && count < 300) {
        PlayerData* p = bpt_search(index_file, current->nome_completo);
        if (p && p->is_retired == 0) {
            strcpy(names_to_delete[count++], current->nome_completo);
        }
        if (p) free(p);
        current = current->proximo;
    }

    if (count == 0) {
        printf("-> Nenhum jogador em atividade para remover em '%s'.\n", country_name);
        return;
    }
    for (int i = 0; i < count; i++) {
        delete_player_by_name(index_file, player_ht, country_ht, names_to_delete[i]);
    }
    printf("-> Operacao concluida. %d jogador(es) em atividade de '%s' foram removidos.\n", count, country_name);
}

void delete_retired_players_by_country(FILE* index_file, HashTable* player_ht, TabelaHashPais* country_ht, const char* country_name) {
    printf("\nIniciando remocao de jogadores APOSENTADOS de '%s'...\n", country_name);
    NoLocalizacaoJogador* player_list = buscar_tabela_hash_pais(country_ht, country_name);

    if (player_list == NULL) {
        printf("-> Nenhum jogador encontrado para o pais '%s'.\n", country_name);
        return;
    }

    char names_to_delete[300][FULL_NAME_SIZE];
    int count = 0;
    NoLocalizacaoJogador* current = player_list;

    while(current != NULL && count < 300) {
        PlayerData* p = bpt_search(index_file, current->nome_completo);
        if (p && p->is_retired == 1) {
            strcpy(names_to_delete[count++], current->nome_completo);
        }
        if (p) free(p);
        current = current->proximo;
    }

    if (count == 0) {
        printf("-> Nenhum jogador aposentado para remover em '%s'.\n", country_name);
        return;
    }
    for (int i = 0; i < count; i++) {
        delete_player_by_name(index_file, player_ht, country_ht, names_to_delete[i]);
    }
    printf("-> Operacao concluida. %d jogador(es) aposentados de '%s' foram removidos.\n", count, country_name);
}



// FUNÇÃO DE BUSCA 2
static void get_nacionality_by_lastname(HashTable* ht, const char* lastname, char* nacionality_out) {
    strcpy(nacionality_out, "N/A"); // Valor padrão caso não encontre

    if (!ht || !lastname) return;

    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashEntry* entry = ht->list_heads[i];
        while (entry != NULL) {
            const char* full_name = entry->full_name;
            int full_name_len = strlen(full_name);
            int lastname_len = strlen(lastname);

            // Compara se o sobrenome bate com o final do nome completo
            if (full_name_len >= lastname_len && strcmp(full_name + full_name_len - lastname_len, lastname) == 0) {
                 if (full_name_len == lastname_len || full_name[full_name_len - lastname_len - 1] == ' ') {
                     LeafNode* leaf = read_leaf_node(entry->leaf_id);
                     if (leaf) {
                         strcpy(nacionality_out, leaf->records[entry->record_index_in_leaf].nacionality);
                         free(leaf);
                         return; // Encontrou, pode sair
                     }
                 }
            }
            entry = entry->next;
        }
    }
}



// --- FUNÇÃO PRINCIPAL DA CONSULTA ---
void find_compatriot_slam_birth_year(HashTableYear* year_ht, HashTable* player_ht) {
    printf("\n--- Verificando jogadores que nasceram em ano de Grand Slam de um compatriota ---\n");

    // PASSO 1: Ler todos os campeões de Grand Slam e armazenar em uma lista
    FILE* fp_champions = fopen("champions.txt", "r");
    if (!fp_champions) {
        perror("Nao foi possivel abrir o arquivo de campeoes");
        return;
    }

    GrandSlamWinner gs_winners[500]; // Array para guardar os campeões. 500 é um número seguro.
    int gs_winner_count = 0;
    char line[1024];
    fgets(line, sizeof(line), fp_champions); // Ignora o cabeçalho

    while (fgets(line, sizeof(line), fp_champions)) {
        char* tokens[20];
        int count = 0;
        char* line_copy = strdup(line);
        char* token = strtok(line_copy, "\\\n");
        while(token != NULL && count < 20) { tokens[count++] = token; token = strtok(NULL, "\\\n"); }

        int year = atoi(tokens[0]);

        // Itera sobre os 4 Grand Slams (posições 1 a 4 no arquivo)
        for (int i = 1; i <= 4; i++) {
            if (count > i && strcmp(tokens[i], "-") != 0) {
                char* winner_lastname = strtok(tokens[i], " ("); // Remove a contagem de títulos (ex: "Lendl (1/1)")
                if (winner_lastname) {
                    gs_winners[gs_winner_count].year = year;
                    strcpy(gs_winners[gs_winner_count].winner_lastname, winner_lastname);
                    // Usa a função auxiliar para buscar a nacionalidade do campeão
                    get_nacionality_by_lastname(player_ht, winner_lastname, gs_winners[gs_winner_count].winner_nacionality);
                    gs_winner_count++;
                }
            }
        }
        free(line_copy);
    }
    fclose(fp_champions);

    // PASSO 2: Para cada campeão, busca na tabela hash e compara
    int found_count = 0;
    for (int i = 0; i < gs_winner_count; i++) {
        GrandSlamWinner* winner = &gs_winners[i];

        // Usa a sua tabela hash para pegar a lista de jogadores nascidos no ano do título
        PlayerLocationNode* candidates_list = search_hash_table_year(year_ht, winner->year);
        
        // Percorre a lista de candidatos (os que nasceram no ano certo)
        while (candidates_list != NULL) {
            // Precisa ler os dados completos do candidato da Árvore B+ para comparar
            LeafNode* leaf = read_leaf_node(candidates_list->leaf_id);
            if (leaf) {
                PlayerData* candidate_player = &leaf->records[candidates_list->record_index_in_leaf];
                
                // Compara a nacionalidade E garante que não é a mesma pessoa
                if (strcmp(candidate_player->nacionality, winner->winner_nacionality) == 0 &&
                    strcmp(candidate_player->lastname, winner->winner_lastname) != 0)
                {
                    printf("-> O jogador %s %s (%s) nasceu em %d, ano em que seu compatriota %s (%s) venceu um Grand Slam.\n",
                        candidate_player->name, candidate_player->lastname, candidate_player->nacionality,
                        winner->year,
                        winner->winner_lastname, winner->winner_nacionality);
                    found_count++;
                }
                free(leaf);
            }
            candidates_list = candidates_list->next;
        }
    }

    if (found_count == 0) {
        printf("Nenhum caso encontrado.\n");
    }
}
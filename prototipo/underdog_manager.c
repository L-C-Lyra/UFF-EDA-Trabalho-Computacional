#ifndef _WIN32
#define _DEFAULT_SOURCE
#endif

#include "underdog_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#if defined(_WIN32)
char* strtok_r(char* str, const char* delim, char** saveptr) {
    return strtok_s(str, delim, saveptr);
}
#endif

#define HASH_SIZE 1009

typedef struct RankNode {
    char name[FULL_NAME_SIZE];
    int best_rank;
    struct RankNode* next;
} RankNode;

typedef struct {
    RankNode* table[HASH_SIZE];
} RankHashTable;

// --- Funções Auxiliares de Hash ---
static unsigned int hash_string(const char* str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % HASH_SIZE;
}

static RankHashTable* create_rank_hash_table() {
    RankHashTable* ht = malloc(sizeof(RankHashTable));
    if (!ht) return NULL;
    for (int i = 0; i < HASH_SIZE; i++) ht->table[i] = NULL;
    return ht;
}

static void insert_rank(RankHashTable* ht, const char* name, int rank) {
    if (!ht || !name) return;
    unsigned int index = hash_string(name);
    RankNode* new_node = malloc(sizeof(RankNode));
    if (!new_node) return;

    strcpy(new_node->name, name);
    new_node->best_rank = rank;
    new_node->next = ht->table[index];
    ht->table[index] = new_node;
}

static int find_rank_smart(RankHashTable* ht, const char* winner_name_token) {
    if (!ht || !winner_name_token) return -1;

    for (int i = 0; i < HASH_SIZE; i++) {
        for (RankNode* node = ht->table[i]; node != NULL; node = node->next) {
            const char* full_name_db = node->name;

            if (strchr(winner_name_token, '.')) {
                char initial = winner_name_token[0];
                const char* lastname_part = strrchr(winner_name_token, ' ');
                if (lastname_part) {
                    lastname_part++;
                    const char* db_lastname_part = strrchr(full_name_db, ' ');
                    if (db_lastname_part) {
                        db_lastname_part++;
                        if (full_name_db[0] == initial && strcmp(lastname_part, db_lastname_part) == 0) {
                            return node->best_rank;
                        }
                    }
                }
            } else {
                size_t full_len = strlen(full_name_db);
                size_t token_len = strlen(winner_name_token);
                if (full_len >= token_len && strcmp(full_name_db + full_len - token_len, winner_name_token) == 0) {
                    if (full_len == token_len || full_name_db[full_len - token_len - 1] == ' ') {
                        return node->best_rank;
                    }
                }
            }
        }
    }
    return -1; // Não encontrado
}

static void free_rank_hash_table(RankHashTable* ht) {
    if (!ht) return;
    for (int i = 0; i < HASH_SIZE; i++) {
        RankNode* node = ht->table[i];
        while (node != NULL) {
            RankNode* temp = node;
            node = node->next;
            free(temp);
        }
    }
    free(ht);
}

// --- Lógica Principal do Relatório ---
void report_underdog_winners(const char* players_filename, const char* champions_filename) {
    RankHashTable* rank_ht = create_rank_hash_table();
    FILE* fp_players = fopen(players_filename, "r");
    if (!fp_players || !rank_ht) {
        perror("Nao foi possivel abrir o arquivo de jogadores ou alocar memoria");
        if(fp_players) fclose(fp_players);
        if(rank_ht) free_rank_hash_table(rank_ht);
        return;
    }

    char line[512];
    fgets(line, sizeof(line), fp_players);

    while(fgets(line, sizeof(line), fp_players)) {
        char line_copy[512];
        strcpy(line_copy, line);
        PlayerData p = parse_player_data(line_copy);
        char full_name[FULL_NAME_SIZE];
        sprintf(full_name, "%s %s", p.name, p.lastname);
        trim_whitespace(full_name);
        if (strlen(full_name) > 0) {
            insert_rank(rank_ht, full_name, p.best_rank);
        }
    }
    fclose(fp_players);

    FILE* fp_champions = fopen(champions_filename, "r");
    if (!fp_champions) {
        perror("Nao foi possivel abrir o arquivo de campeoes");
        free_rank_hash_table(rank_ht);
        return;
    }

    printf("\n\n--- RELATORIO: JOGADORES QUE 'FURARAM O RANKING' ---\n");
    printf("(Venceram grandes torneios sem nunca ter entrado no Top 25 da carreira)\n");

    fgets(line, sizeof(line), fp_champions);

    while (fgets(line, sizeof(line), fp_champions)) {
        char line_copy[1024];
        strcpy(line_copy, line);

        char* save_ptr;
        char* parts[20];
        int part_count = 0;
        char* token = strtok_r(line_copy, "\\\n", &save_ptr);
        while(token != NULL && part_count < 20) {
            parts[part_count++] = token;
            token = strtok_r(NULL, "\\\n", &save_ptr);
        }

        if (part_count < 1) continue;
        char* year_str = parts[0];

        for (int i = 1; i < part_count; i++) {
            char* name_token_base = strtok(parts[i], "(");
            if (!name_token_base) continue;

            char name_clean[FULL_NAME_SIZE];
            strcpy(name_clean, name_token_base);
            trim_whitespace(name_clean);

            if (strlen(name_clean) == 0 || strcmp(name_clean, "-") == 0) continue;

            int rank = find_rank_smart(rank_ht, name_clean);

            if (rank > 25 || rank == -1) {
                const char* tournament_type = "Outro";
                if (i >= 1 && i <= 4) tournament_type = "Grand Slam";
                else if (i >= 7 && i <= 15) tournament_type = "ATP 1000";
                else if (i == 5) tournament_type = "ATP Finals";
                else if (i == 6) tournament_type = "Olimpiadas";

                if (strcmp(tournament_type, "Outro") != 0) {
                     printf("  - Ano: %s | Jogador: %-25s | Categoria: %s (Melhor Rank: %d)\n", year_str, name_clean, tournament_type, rank);
                }
            }
        }
    }

    fclose(fp_champions);
    free_rank_hash_table(rank_ht);
}
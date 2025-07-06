#include "title_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int hash_function(const char *str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % STATS_TABLE_SIZE;
}

PlayerStatsHashTable* create_stats_table() {
    PlayerStatsHashTable* ht = malloc(sizeof(PlayerStatsHashTable));
    for (int i = 0; i < STATS_TABLE_SIZE; i++) {
        ht->table[i] = NULL;
    }
    return ht;
}

void free_stats_table(PlayerStatsHashTable* ht) {
    for (int i = 0; i < STATS_TABLE_SIZE; i++) {
        PlayerStatsNode* node = ht->table[i];
        while (node != NULL) {
            PlayerStatsNode* temp = node;
            node = node->next;
            free(temp);
        }
    }
    free(ht);
}

void create_player_entry(PlayerStatsHashTable* ht, const char* full_name, int is_retired_status) {
    unsigned int index = hash_function(full_name);
    PlayerStatsNode* new_node = malloc(sizeof(PlayerStatsNode));

    strcpy(new_node->stats.full_name, full_name);
    new_node->stats.is_retired = is_retired_status;
    new_node->stats.total_titles = 0;
    for (int i = 0; i < NUM_TOURNAMENTS; i++) {
        new_node->stats.titles[i] = 0;
    }

    new_node->next = ht->table[index];
    ht->table[index] = new_node;
}

PlayerStats* find_stats_by_lastname(PlayerStatsHashTable* ht, const char* lastname) {
    for (int i = 0; i < STATS_TABLE_SIZE; i++) {
        PlayerStatsNode* node = ht->table[i];
        while (node != NULL) {
            const char* full_name = node->stats.full_name;
            size_t full_name_len = strlen(full_name);
            size_t lastname_len = strlen(lastname);

            if (full_name_len >= lastname_len &&
                strcmp(full_name + full_name_len - lastname_len, lastname) == 0) {
                if (full_name_len == lastname_len || full_name[full_name_len - lastname_len - 1] == ' ') {
                    return &node->stats;
                }
                }
            node = node->next;
        }
    }
    return NULL;
}

void increment_title(PlayerStatsHashTable* ht, char* winner_lastname, Tournament tournament) {
    if (winner_lastname == NULL || strcmp(winner_lastname, "-") == 0 || strlen(winner_lastname) < 2) return;
    char* trimmed_name = strtok(winner_lastname, " \r\n()");
    if (!trimmed_name) return;

    PlayerStats* stats = find_stats_by_lastname(ht, trimmed_name);

    if (stats) {
        stats->titles[tournament]++;
        stats->total_titles++;
    }
}

int compare_players(const void* a, const void* b) {
    PlayerStats* p1 = (PlayerStats*)a;
    PlayerStats* p2 = (PlayerStats*)b;

    for (int i = 0; i < NUM_TOURNAMENTS; i++) {
        if (p1->titles[i] != p2->titles[i]) {
            return p2->titles[i] - p1->titles[i];
        }
    }

    return strcmp(p1->full_name, p2->full_name);
}

void report_titles_by_status(FILE *bpt_index_file, const char* champions_filename) {
    PlayerStatsHashTable* stats_table = create_stats_table();

    BPTHeader *header = read_header(bpt_index_file);
    if (!header) {
        printf("Nao foi possivel ler o header da Arvore B+.\n");
        free_stats_table(stats_table);
        return;
    }

    int total_players = 0;
    int current_leaf_id = -1;
    while(current_leaf_id != 0) {
        LeafNode *leaf = read_leaf_node(current_leaf_id);
        if (!leaf) break;
        for (int i = 0; i < leaf->num_records; i++) {
            PlayerData* p = &leaf->records[i];
            char full_name[NAME_SIZE * 2];
            sprintf(full_name, "%s %s", p->name, p->lastname);
            create_player_entry(stats_table, full_name, p->is_retired);
            total_players++;
        }
        current_leaf_id = leaf->next_leaf_id;
        free(leaf);
    }
    rewind(bpt_index_file);

    FILE* fp_champions = fopen(champions_filename, "r");
    if (!fp_champions) { /*...*/ return; }

    char line[1024];
    fgets(line, sizeof(line), fp_champions);

    while (fgets(line, sizeof(line), fp_champions)) {
        char* tokens[20];
        int count = 0;
        char* token = strtok(line, "\\\n");
        while(token != NULL && count < 20) { tokens[count++] = token; token = strtok(NULL, "\\\n"); }

        if (count > 1) increment_title(stats_table, tokens[1], AUSTRALIAN_OPEN);
        if (count > 2) increment_title(stats_table, tokens[2], ROLAND_GARROS);
        if (count > 3) increment_title(stats_table, tokens[3], WIMBLEDON);
        if (count > 4) increment_title(stats_table, tokens[4], US_OPEN);
        if (count > 5) increment_title(stats_table, tokens[5], ATP_FINALS);
        if (count > 6) increment_title(stats_table, tokens[6], OLYMPICS);
        if (count > 7) increment_title(stats_table, tokens[7], INDIAN_WELLS);
        if (count > 8) increment_title(stats_table, tokens[8], MIAMI);
        if (count > 9) increment_title(stats_table, tokens[9], MONTE_CARLO);
        if (count > 10) increment_title(stats_table, tokens[10], MADRID);
        if (count > 11) increment_title(stats_table, tokens[11], ROME);
        if (count > 12) increment_title(stats_table, tokens[12], CANADA);
        if (count > 13) increment_title(stats_table, tokens[13], CINCINNATI);
        if (count > 14) increment_title(stats_table, tokens[14], SHANGHAI);
        if (count > 15) increment_title(stats_table, tokens[15], PARIS);
    }
    fclose(fp_champions);

    PlayerStats active_players[total_players];
    int active_count = 0;
    PlayerStats retired_players[total_players];
    int retired_count = 0;

    for(int i = 0; i < STATS_TABLE_SIZE; i++) {
        PlayerStatsNode* node = stats_table->table[i];
        while(node != NULL) {
            if (node->stats.total_titles > 0) {
                if (node->stats.is_retired == 1) retired_players[retired_count++] = node->stats;
                else if (node->stats.is_retired == 0) active_players[active_count++] = node->stats;
            }
            node = node->next;
        }
    }
    qsort(active_players, active_count, sizeof(PlayerStats), compare_players);
    qsort(retired_players, retired_count, sizeof(PlayerStats), compare_players);

    // PASSO 4: Imprimir o relatório
    printf("\n\n=======================================================\n");
    printf("         RELATORIO DE TITULOS POR STATUS\n");
    printf("=======================================================\n");

    printf("\n--- JOGADORES APOSENTADOS COM TITULOS: %d ---\n", retired_count);
    for (int i = 0; i < retired_count; i++) {
        printf("%-25s | Total: %2d (GS: %d, 1000s: %d, Finals: %d, Oly: %d)\n",
            retired_players[i].full_name,
            retired_players[i].total_titles,
            (retired_players[i].titles[AUSTRALIAN_OPEN] + retired_players[i].titles[ROLAND_GARROS] + retired_players[i].titles[WIMBLEDON] + retired_players[i].titles[US_OPEN]),
            (retired_players[i].titles[INDIAN_WELLS] + retired_players[i].titles[MIAMI] + retired_players[i].titles[MONTE_CARLO] + retired_players[i].titles[MADRID] + retired_players[i].titles[ROME] + retired_players[i].titles[CANADA] + retired_players[i].titles[CINCINNATI] + retired_players[i].titles[SHANGHAI] + retired_players[i].titles[PARIS]),
            retired_players[i].titles[ATP_FINALS],
            retired_players[i].titles[OLYMPICS]);
    }
    printf("\n--- JOGADORES ATIVOS COM TITULOS: %d ---\n", active_count);
    for (int i = 0; i < active_count; i++) {
        printf("%-25s | Total: %2d (GS: %d, 1000s: %d, Finals: %d, Oly: %d)\n",
            active_players[i].full_name,
            active_players[i].total_titles,
            (active_players[i].titles[AUSTRALIAN_OPEN] + active_players[i].titles[ROLAND_GARROS] + active_players[i].titles[WIMBLEDON] + active_players[i].titles[US_OPEN]),
            (active_players[i].titles[INDIAN_WELLS] + active_players[i].titles[MIAMI] + active_players[i].titles[MONTE_CARLO] + active_players[i].titles[MADRID] + active_players[i].titles[ROME] + active_players[i].titles[CANADA] + active_players[i].titles[CINCINNATI] + active_players[i].titles[SHANGHAI] + active_players[i].titles[PARIS]),
            active_players[i].titles[ATP_FINALS],
            active_players[i].titles[OLYMPICS]);
    }

    printf("\n");

    free(header);
    free_stats_table(stats_table);
}
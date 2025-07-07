#include "ranking_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


// Estrutura auxiliar para a tabela hash de jogadores
typedef struct PlayerNode {
    char name[FULL_NAME_SIZE];
    YearlyPerformance performances[MAX_YEARS];
    int perf_count;
    struct PlayerNode* next;
} PlayerNode;

typedef struct {
    PlayerNode* table[STATS_TABLE_SIZE];
} PlayerHashTable;

// --- Funções Auxiliares de Hash ---

static unsigned int hash_player_name(const char* str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % STATS_TABLE_SIZE;
}

static PlayerHashTable* create_player_hash_table() {
    PlayerHashTable* ht = malloc(sizeof(PlayerHashTable));
    if (ht == NULL) {
        perror("Falha ao alocar memoria para PlayerHashTable");
        return NULL;
    }
    for (int i = 0; i < STATS_TABLE_SIZE; i++) ht->table[i] = NULL;
    return ht;
}

static PlayerNode* get_or_create_player(PlayerHashTable* ht, const char* name) {
    if (!ht || !name) return NULL;
    unsigned int index = hash_player_name(name);
    PlayerNode* node = ht->table[index];
    while (node != NULL) {
        if (strcmp(node->name, name) == 0) return node;
        node = node->next;
    }

    PlayerNode* new_node = malloc(sizeof(PlayerNode));
    if (!new_node) return NULL;

    strcpy(new_node->name, name);
    new_node->perf_count = 0;
    new_node->next = ht->table[index];
    ht->table[index] = new_node;
    return new_node;
}

static YearlyPerformance* get_performance(PlayerNode* player, int year) {
    if (!player) return NULL;
    for (int i = 0; i < player->perf_count; ++i) {
        if (player->performances[i].year == year) {
            return &player->performances[i];
        }
    }
    return NULL;
}

static void free_player_hash_table(PlayerHashTable* ht) {
    if (!ht) return;
    for (int i = 0; i < STATS_TABLE_SIZE; i++) {
        PlayerNode* node = ht->table[i];
        while (node != NULL) {
            PlayerNode* temp = node;
            node = node->next;
            free(temp);
        }
    }
    free(ht);
}

// --- Lógica Principal ---

static int compare_performances(const void* a, const void* b) {
    YearlyPerformance* p1 = (YearlyPerformance*)a;
    YearlyPerformance* p2 = (YearlyPerformance*)b;
    if (p1->points != p2->points) return p2->points - p1->points;
    if (p1->grand_slams != p2->grand_slams) return p2->grand_slams - p1->grand_slams;
    if (p1->atp_1000s != p2->atp_1000s) return p2->atp_1000s - p1->atp_1000s;
    if (p1->previous_year_points != p2->previous_year_points) return p2->previous_year_points - p1->previous_year_points;
    return strcmp(p1->player_name, p2->player_name);
}

static int compare_winners(const void* a, const void* b) {
    TournamentWinner* w1 = (TournamentWinner*)a;
    TournamentWinner* w2 = (TournamentWinner*)b;
    if (w1->win_count != w2->win_count) return w2->win_count - w1->win_count;
    return strcmp(w1->player_name, w2->player_name);
}

void report_atp_finals_participants_by_year(const char* champions_filename) {
    FILE* fp = fopen(champions_filename, "r");
    if (!fp) {
        perror("Nao foi possivel abrir o arquivo de campeoes");
        return;
    }

    PlayerHashTable* ht = create_player_hash_table();
    if (!ht) { fclose(fp); return; }

    char line[1024];
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
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

        if (part_count < 16) continue;
        int year = atoi(parts[0]);

        for (int i = 1; i < part_count; i++) {
            char* name_token = strtok(parts[i], "(");
            if (!name_token) continue;

            char* name = name_token;
            while(isspace((unsigned char)*name)) name++;
            char* end = name + strlen(name) - 1;
            while(end > name && isspace((unsigned char)*end)) end--;
            *(end + 1) = 0;

            if (strcmp(name, "-") == 0 || strlen(name) == 0) continue;

            PlayerNode* player = get_or_create_player(ht, name);
            if (!player) continue;

            YearlyPerformance* perf = get_performance(player, year);
            if(perf == NULL) {
                if(player->perf_count >= MAX_YEARS) continue;
                perf = &player->performances[player->perf_count++];
                memset(perf, 0, sizeof(YearlyPerformance));
                perf->year = year;
                strcpy(perf->player_name, name);
            }

            if (i >= 1 && i <= 4) { perf->points += 2000; perf->grand_slams++; }
            else if (i >= 7 && i <= 15) { perf->points += 1000; perf->atp_1000s++; }
            else if (i == 5) { perf->points += 1500; }
            else if (i == 6) { perf->points += 800; }
        }
    }

    printf("\n\n--- PARTICIPANTES DO ATP FINALS POR ANO (COM DESEMPATE) ---\n");
    printf("1990: Stefan Edberg, Andre Agassi, Pete Sampras, Emilio Sanchez, Boris Becker, Ivan Lendl, Andres Gomez, Thomas Muster.\n");

    for (int year = 1991; year <= 2024; year++) {
        YearlyPerformance year_performances[MAX_PLAYERS];
        int year_perf_count = 0;

        for (int i = 0; i < STATS_TABLE_SIZE; i++) {
            PlayerNode* node = ht->table[i];
            while (node != NULL) {
                YearlyPerformance* current_year_perf = get_performance(node, year);
                if (current_year_perf) {
                    YearlyPerformance* prev_year_perf = get_performance(node, year - 1);
                    current_year_perf->previous_year_points = prev_year_perf ? prev_year_perf->points : 0;
                    if(year_perf_count < MAX_PLAYERS) {
                        year_performances[year_perf_count++] = *current_year_perf;
                    }
                }
                node = node->next;
            }
        }

        if (year_perf_count > 0) {
            qsort(year_performances, year_perf_count, sizeof(YearlyPerformance), compare_performances);
            printf("\n%d:\n", year);
            for (int i = 0; i < 8 && i < year_perf_count; i++) {
                printf("  %d. %-25s (Pontos: %d, GS: %d, 1000s: %d, Pts Ano Ant: %d)\n",
                    i + 1, year_performances[i].player_name, year_performances[i].points,
                    year_performances[i].grand_slams, year_performances[i].atp_1000s,
                    year_performances[i].previous_year_points);
            }
        }
    }

    free_player_hash_table(ht);
    fclose(fp);
}

void report_tournament_winners(const char* champions_filename) {
    char tournament_names[NUM_TOURNAMENTS][50] = {
        "Australian Open", "Roland Garros", "Wimbledon", "US Open", "ATP Finals",
        "Olympics", "Indian Wells", "Miami", "Monte Carlo", "Madrid", "Rome",
        "Canada", "Cincinnati", "Shanghai", "Paris"
    };

    for (int t_idx = 0; t_idx < 15; t_idx++) {
        FILE* fp = fopen(champions_filename, "r");
        if (!fp) continue;

        PlayerHashTable* ht = create_player_hash_table();
        if (!ht) { fclose(fp); continue; }

        char line[1024];
        fgets(line, sizeof(line), fp);

        while (fgets(line, sizeof(line), fp)) {
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

            if (part_count <= t_idx + 1) continue;

            char* name_token = strtok(parts[t_idx + 1], "(");
            if (!name_token) continue;

            char* name = name_token;
            while(isspace((unsigned char)*name)) name++;
            char* end = name + strlen(name) - 1;
            while(end > name && isspace((unsigned char)*end)) end--;
            *(end + 1) = 0;

            if (strcmp(name, "-") == 0 || strlen(name) == 0) continue;

            PlayerNode* player = get_or_create_player(ht, name);
            if (!player) continue;

            if (player->perf_count == 0) {
                 player->performances[0].points = 0;
                 player->perf_count = 1;
            }
            player->performances[0].points++;
        }

        TournamentWinner winners[MAX_PLAYERS];
        int winner_count = 0;
        for (int i = 0; i < STATS_TABLE_SIZE; i++) {
            PlayerNode* node = ht->table[i];
            while (node != NULL) {
                if (node->perf_count > 0) {
                    if (winner_count < MAX_PLAYERS) {
                        strcpy(winners[winner_count].player_name, node->name);
                        winners[winner_count].win_count = node->performances[0].points;
                        winner_count++;
                    }
                }
                node = node->next;
            }
        }

        qsort(winners, winner_count, sizeof(TournamentWinner), compare_winners);

        printf("\n\n--- VENCEDORES PARA: %s ---\n", tournament_names[t_idx]);
        for(int i = 0; i < winner_count; i++) {
            printf("%-25s | Vitorias: %d\n", winners[i].player_name, winners[i].win_count);
        }

        free_player_hash_table(ht);
        fclose(fp);
    }
}
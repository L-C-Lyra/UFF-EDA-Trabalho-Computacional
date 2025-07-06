#ifndef TITLE_MANAGER_H
#define TITLE_MANAGER_H

#include "bplus_tree_io.h"


typedef enum {
    AUSTRALIAN_OPEN,    // 0
    ROLAND_GARROS,      // 1
    WIMBLEDON,          // 2
    US_OPEN,            // 3
    ATP_FINALS,         // 4
    OLYMPICS,           // 5
    INDIAN_WELLS,       // 6
    MIAMI,              // 7
    MONTE_CARLO,        // 8
    MADRID,             // 9
    ROME,               // 10
    CANADA,             // 11
    CINCINNATI,         // 12
    SHANGHAI,           // 13
    PARIS,              // 14
    NUM_TOURNAMENTS     // 15 - Total de torneios para contar
} Tournament;

typedef struct PlayerStats {
    char full_name[NAME_SIZE * 2];
    int is_retired;
    int titles[NUM_TOURNAMENTS];
    int total_titles;
} PlayerStats;

typedef struct PlayerStatsNode {
    PlayerStats stats;
    struct PlayerStatsNode *next;
} PlayerStatsNode;

#define STATS_TABLE_SIZE 1009
typedef struct {
    PlayerStatsNode* table[STATS_TABLE_SIZE];
} PlayerStatsHashTable;

void report_titles_by_status(FILE *bpt_index_file, const char* champions_filename);

#endif //TITLE_MANAGER_H
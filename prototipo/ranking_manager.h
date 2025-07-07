#ifndef RANKING_MANAGER_H
#define RANKING_MANAGER_H

#include "title_manager.h"

#define MAX_PLAYERS 300
#define MAX_YEARS 50

typedef struct {
    char player_name[FULL_NAME_SIZE];
    int year;
    int points;
    int grand_slams;
    int atp_1000s;
    int atp_finals_win;
    int previous_year_points;
} YearlyPerformance;

typedef struct {
    char player_name[FULL_NAME_SIZE];
    int win_count;
} TournamentWinner;

void report_atp_finals_participants_by_year(const char* champions_filename);

void report_tournament_winners(const char* champions_filename);

#endif // RANKING_MANAGER_H
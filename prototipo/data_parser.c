#include "data_parser.h"
#include <string.h> 
#include <stdio.h>
#include <stdlib.h> // pra atoi
#include <ctype.h>  // pra isspace



// funcao aux para remover espaços e quebras de linha
void trim_whitespace(char *str) {
    int i;
    int begin = 0;
    int end = strlen(str) - 1;

    while (isspace((unsigned char)str[begin]))
        begin++;

    while ((end >= begin) && isspace((unsigned char)str[end]))
        end--;

    for (i = begin; i <= end; i++)
        str[i - begin] = str[i];

    str[i - begin] = '\0';
}


PlayerData parse_player_data(char *line) {
    PlayerData player;
    memset(&player, 0, sizeof(PlayerData)); 

    char *token;


    // nome
    token = strtok(line, "\\"); //  passa a string original
    if (token) {
        trim_whitespace(token);
        char *name_end = strchr(token, ' ');
        if (name_end) {
            strncpy(player.name, token, name_end - token);
            player.name[name_end - token] = '\0';
            strcpy(player.lastname, name_end + 1);
        } else {
            strcpy(player.name, token);
            strcpy(player.lastname, ""); // Sem sobrenome
        }
    }

    // ano de nascimento
    token = strtok(NULL, "\\"); // Chamadas subsequentes: passa NULL
    if (token && strcmp(token, "-") != 0) {
        player.birth_year = atoi(token);
    } else {
        player.birth_year = -1;
    }

    // ano da morte
    token = strtok(NULL, "\\"); 
    if (token && strcmp(token, "-") != 0) {
        player.death_year = atoi(token);
    } else {
        player.death_year = -1;
    }

    // nacionalidade
    token = strtok(NULL, "\\"); 
    if (token) {
        trim_whitespace(token);
        strcpy(player.nacionality, token);
    }

    // melhor ranking
    token = strtok(NULL, "\\"); 
    if (token && strcmp(token, "-") != 0) {
        char *rank_end = strchr(token, '(');
        if (rank_end) {
            *rank_end = '\0';
        }
        player.best_rank = atoi(token);
    } else {
        player.best_rank = -1;
    }

    // ano do melhor ranking
    token = strtok(NULL, "\n"); 
    if (token && strcmp(token, "-") != 0) {
        char *year_end = strchr(token, '(');
        if (year_end) {
            *year_end = '\0';
        }
        player.best_rank_year = atoi(token);
    } else {
        player.best_rank_year = -1;
    } 

    player.points = 0;

    return player;
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "bplus_tree_io.h" 


static void parse_winner_name(const char* raw_token, char* clean_name_out, int buffer_size) {
    if (raw_token == NULL || strcmp(raw_token, "-") == 0) {
        strcpy(clean_name_out, "-");
        return;
    }

    const char *end_of_name = strchr(raw_token, '(');
    int len;

    if (end_of_name) {
        len = end_of_name - raw_token;
    } else {
        len = strlen(raw_token);
    }
    
    
    strncpy(clean_name_out, raw_token, len);
    clean_name_out[len] = '\0';

   
    int end_idx = strlen(clean_name_out) - 1;
    while(end_idx >= 0 && isspace((unsigned char)clean_name_out[end_idx])) {
        clean_name_out[end_idx] = '\0';
        end_idx--;
    }
}



void report_calendar_grand_slam() {
    printf("\n");
    printf(" RELATÓRIO: Vencedores de todos os Grand Slams no mesmo ano:\n");
    printf("\n");

    FILE *fp_champions = fopen("champions.txt", "r");
    if (fp_champions == NULL) {
        perror("Erro ao abrir champions.txt");
        return;
    }

    char line[1024];
    int found_count = 0;
    
   
    fgets(line, sizeof(line), fp_champions);

    
    while (fgets(line, sizeof(line), fp_champions) != NULL) {
        char* line_ptr = line;
        char* token;
        int column = 0;

        int year = 0;
        char gs_winners[4][NAME_SIZE]; // Array para guardar os 4 vencedores de GS do ano

        
        while ((token = strtok_r(line_ptr, "\\\n", &line_ptr))) {
            if (column == 0) { // Coluna do ano
                year = atoi(token);
            } 
            else if (column >= 1 && column <= 4) { // Colunas dos 4 Grand Slams
                parse_winner_name(token, gs_winners[column - 1], NAME_SIZE);
            }
            column++;
        }

        
        if (strcmp(gs_winners[0], "-") != 0 &&
            strcmp(gs_winners[0], gs_winners[1]) == 0 &&
            strcmp(gs_winners[0], gs_winners[2]) == 0 &&
            strcmp(gs_winners[0], gs_winners[3]) == 0) 
        {
            printf("-> CONQUISTA ENCONTRADA!\n");
            printf("   Ano: %d\n", year);
            printf("   Jogador: %s\n\n", gs_winners[0]);
            found_count++;
        }
    }

    if (found_count == 0) {
        printf("Nenhum jogador conquistou os quatro Grand Slams no mesmo ano no período analisado (1990 a 2024).\n");
    }

    fclose(fp_champions);
}

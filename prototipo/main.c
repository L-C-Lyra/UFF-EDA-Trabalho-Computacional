#include "bplus_tree_io.h" 
#include "data_parser.h"   
#include <stdio.h>         
#include <stdlib.h>        

int main() {
    
    FILE *fp_index = fopen("indices_t.bin", "wb");
    if (fp_index == NULL) {
        perror("Erro ao abrir indices_t.bin para escrita");
        return 1;
    }

    init_BPT(fp_index);
    fclose(fp_index); 

    fp_index = fopen("indices_t.bin", "rb");
    if (fp_index == NULL) {
        perror("Erro ao reabrir indices_t.bin para leitura");
        return 1;
    }

 
    BPTHeader *file_header = read_header(fp_index);
    print_header(file_header);

    printf("\n");

    print_internal_nodes(fp_index);


    LeafNode *leaf_node = read_leaf_node(-1);
    print_leaf_aux(leaf_node);

    fclose(fp_index); 
    free(file_header);
    free(leaf_node);

   
    // parsing de dados dos players
    printf("\n--- Carregando e Parseando Dados de tennis_players.txt ---\n");

    FILE *fp_players = fopen("tennis_players.txt", "r");
    if (fp_players == NULL) {
        perror("Erro ao abrir tennis_players.txt");
        return 1;
    }

    char line[512]; 
    int line_count = 0;


    if (fgets(line, sizeof(line), fp_players) != NULL) {
        printf("Cabeçalho do arquivo de jogadores ignorado: %s", line);
    } else {
        printf("Arquivo tennis_players.txt está vazio ou erro na leitura do cabeçalho.\n");
        fclose(fp_players);
        return 1;
    }


    printf("Dados de Jogadores Parseados:\n");
    while (fgets(line, sizeof(line), fp_players) != NULL) {
        line_count++;
        if (line[0] == '[') {
            printf("Ignorando linha de source: %s", line);
            continue;
        }

        PlayerData p = parse_player_data(line);


        printf("  Linha %d - Nome: %s %s, Nasc: %d, Rank: %d\n",
               line_count, p.name, p.lastname, p.birth_year, p.best_rank);


    }

    fclose(fp_players); 

    printf("\n--- Carregamento e Parsing Concluídos. Próximo passo: Implementar a Inserção na B+ Tree ---\n");


    return 0;
}
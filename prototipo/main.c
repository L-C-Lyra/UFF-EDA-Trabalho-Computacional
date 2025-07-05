#include "bplus_tree.h" 
#include "data_parser.h"   
#include <stdio.h>         
#include <stdlib.h>        

int main() {
    
    // Passo 1: Cria (ou zera) o arquivo de índice.
    FILE *fp_index = fopen("indices_t.bin", "wb");
    if (fp_index == NULL) {
        perror("Erro ao criar indices_t.bin");
        return 1;
    }
    init_BPT(fp_index, BTREE_ORDER);
    fclose(fp_index);

    // Passo 2: Parse os dados dos jogadores do arquivo texto.
    printf("\n--- Carregando e Parseando Dados de tennis_players.txt ---\n");
    FILE *fp_players = fopen("tennis_players.txt", "r");
    if (fp_players == NULL) {
        perror("Erro ao abrir tennis_players.txt");
        return 1;
    }
    PlayerData players[310];
    char line[512];
    int player_count = 0;
    fgets(line, sizeof(line), fp_players); // Ignora cabeçalho
    while (fgets(line, sizeof(line), fp_players) != NULL && player_count < 310) {
        if (line[0] == '[' || line[0] == '\n' || strncmp(line, "Ano", 3) == 0) continue;
        players[player_count] = parse_player_data(line);
        player_count++;
    }
    fclose(fp_players);
    printf("Total de %d jogadores parseados.\n", player_count);

    // Passo 3: Insere cada jogador na Árvore B+ em memória secundária.
    printf("\n--- Iniciando Inserção na Árvore B+ (em disco) ---\n");

    // Abre o arquivo no modo "r+b" para permitir leitura e escrita no mesmo arquivo.
    fp_index = fopen("indices_t.bin", "r+b");
    if (fp_index == NULL) {
        perror("Erro ao reabrir indices_t.bin para inserção");
        return 1;
    }

    for (int i = 0; i < player_count; i++) {
        // Ignora jogadores com nome vazio que podem vir do parsing.
        if(strlen(players[i].lastname) > 0) {
             printf("Inserindo jogador: %s\n", players[i].lastname);
             bpt_insert(fp_index, players[i]);
        }
    }
    fclose(fp_index); // Fecha o arquivo após todas as inserções.
    printf("\n--- Inserção em Disco Concluída ---\n");

    // Passo 4: Exibe o estado final da árvore para verificação.
    printf("\n--- Verificando Estado Final da Árvore B+ ---\n");
    fp_index = fopen("indices_t.bin", "rb");
    if (fp_index == NULL) {
        perror("Erro ao abrir indices_t.bin para leitura final");
        return 1;
    }
    BPTHeader *file_header = read_header(fp_index);
    if(file_header){
        print_header(file_header);
        printf("\n--- Nós Internos ---\n");
        print_internal_nodes(fp_index);
        printf("\n--- Nós Folha (arquivos separados) ---\n");
        print_leafs(file_header);
        free(file_header);
    }
    fclose(fp_index);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bplus_tree_io.h"
#include "data_parser.h"
#include "hash_table.h"
#include "tabela_hash_pais.h"

int main() {
   
    printf("Criando tabelas hash em memoria..\n");
    HashTable *player_hash_table = create_hash_table();
    TabelaHashPais *tabela_hash_paises = criar_tabela_hash_pais();

    if (player_hash_table == NULL || tabela_hash_paises == NULL) {
        printf("Falha ao criar uma das tabelas hash.\n");
        return 1;
    }
    printf("Tabelas hash criadas com sucesso.\n");


   
    printf("\n--- Carregando dados de tennis_players.txt ---\n");
    FILE *fp_players = fopen("tennis_players.txt", "r");
    if (fp_players == NULL) {
        perror("Erro ao abrir tennis_players.txt");
        return 1;
    }

    char line[512];
    fgets(line, sizeof(line), fp_players); // ignora cabeçalho

    int actual_leaf_id = -1;
    int actual_record_index = 0;

    while (fgets(line, sizeof(line), fp_players) != NULL) {
        if (line[0] == '[') continue;

        char line_copy[512];
        strcpy(line_copy, line);

        PlayerData p = parse_player_data(line_copy);

        char full_name[NAME_SIZE * 2];
        sprintf(full_name, "%s %s", p.name, p.lastname);

        // insere nas duas tabelas hash
        inserir_tabela_hash_pais(tabela_hash_paises, p.nacionality, full_name, actual_leaf_id, actual_record_index);
        hash_table_insert(player_hash_table, full_name, actual_leaf_id, actual_record_index);

        actual_record_index++;
    }
    fclose(fp_players);
    printf("--- Dados carregados nas tabelas hash. ---\n");

    // menu
    int choice;
    char search_string[100];

    do {
        printf("\n----- MENU DE TESTE -----\n");
        printf("1. Buscar jogador por NOME\n");
        printf("2. Buscar jogadores por PAIS\n");
        printf("3. Sair\n");
        printf("Escolha uma opçao: ");

        if (scanf("%d", &choice) != 1) {
            choice = 0;
        }
        while (getchar() != '\n');

        switch (choice) {
            
            case 1: {
                printf("\nDigite o nome completo do jogador (Ex: Roger Federer): ");
                if (fgets(search_string, sizeof(search_string), stdin) != NULL) {
                    search_string[strcspn(search_string, "\n")] = '\0';

                    int found_leaf_id, found_record_index;
                    // chama a função de busca da hash por nome
                    if (hash_table_search(player_hash_table, search_string, &found_leaf_id, &found_record_index)) {
                        printf(">>> SUCESSO: Jogador '%s' encontrado!\n", search_string);
                        printf("    (Localização simulada: Folha %d, Registro %d)\n", found_leaf_id, found_record_index);
                    } else {
                        printf(">>> FALHA: Jogador '%s' não encontrado na tabela hash.\n", search_string);
                    }
                }
                break;
            }

            case 2: {
                printf("\nDigite a nacionalidade para buscar (Ex: Spain): ");
                if (fgets(search_string, sizeof(search_string), stdin) != NULL) {
                    search_string[strcspn(search_string, "\n")] = '\0';

                    NoLocalizacaoJogador *player_list = buscar_tabela_hash_pais(tabela_hash_paises, search_string);

                    if (player_list == NULL) {
                        printf("Nenhum jogador encontrado para o pais: '%s'\n", search_string);
                    } else {
                        printf("Jogadores encontrados para '%s':\n", search_string);
                        NoLocalizacaoJogador *current = player_list;
                        while (current != NULL) {
                            printf("  - %s\n", current->nome_completo);
                            current = current->proximo;
                        }
                    }
                }
                break;
            }

            case 3:
                printf("Encerrando...\n");
                break;

            default:
                printf("Opçao invalida. Tente novamente.\n");
                break;
        }

    } while (choice != 3);

    
    free_hash_table(player_hash_table);
    liberar_tabela_hash_pais(tabela_hash_paises);

    return 0;
}
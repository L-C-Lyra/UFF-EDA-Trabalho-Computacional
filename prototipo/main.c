#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bplus_tree.h"
#include "bplus_tree_io.h"
#include "data_parser.h"
#include "hash_table.h"
#include "tabela_hash_pais.h"
#include "title_manager.h"
#include "operations.h"

void show_menu(FILE* fp_index, HashTable* player_ht, TabelaHashPais* country_ht);

int main() {
    printf("=======================================================\n");
    printf("        INICIALIZANDO TRABALHO COMPUTACIONAL\n");
    printf("=======================================================\n\n");

    FILE *fp_index = fopen("indices_t.bin", "wb+");
    if (fp_index == NULL) { perror("Erro critico ao criar/abrir 'indices_t.bin'"); return 1; }
    init_BPT(fp_index, BTREE_ORDER);

    HashTable* player_ht = create_hash_table();
    TabelaHashPais* country_ht = criar_tabela_hash_pais();

    FILE *fp_players = fopen("tennis_players.txt", "r");
    if (fp_players == NULL) { perror("Erro ao abrir 'tennis_players.txt'"); fclose(fp_index); return 1; }

    char line[512];
    int player_count = 0;
    fgets(line, sizeof(line), fp_players);

    while (fgets(line, sizeof(line), fp_players) != NULL) {
        if (strlen(line) < 10) continue;
        PlayerData p = parse_player_data(line);
        if (p.birth_year > 0) {
            PlayerLocation loc = bpt_insert(fp_index, p);
            if (loc.leaf_id != -1) {
                char full_name[NAME_SIZE * 2];
                sprintf(full_name, "%s %s", p.name, p.lastname);
                hash_table_insert(player_ht, full_name, loc.leaf_id, loc.record_index);
                inserir_tabela_hash_pais(country_ht, p.nacionality, full_name, loc.leaf_id, loc.record_index);
                player_count++;
            }
        }
    }
    printf("-> Estruturas de dados construidas com %d jogadores.\n", player_count);
    fclose(fp_players);

    show_menu(fp_index, player_ht, country_ht);

    printf("\n=======================================================\n");
    printf("                 EXECUCAO FINALIZADA\n");
    printf("=======================================================\n");

    fclose(fp_index);
    free_hash_table(player_ht);
    liberar_tabela_hash_pais(country_ht);
    return 0;
}

void show_menu(FILE *fp_index, HashTable* player_ht, TabelaHashPais* country_ht) {
    int choice = 0;
    char input_buffer[100];
    char name_buffer[NAME_SIZE * 2];

    do {
        printf("\n\n=============== MENU DE OPCOES ===============\n");
        printf("--- Operacoes de Modificacao ---\n");
        printf(" 1. Inserir Novo Jogador\n");
        printf(" 2. Remover Jogador por Nome\n");
        printf(" 3. Remover TODOS os Jogadores de um Pais\n");
        printf("--- Operacoes de Busca e Listagem ---\n");
        printf(" 4. Buscar Jogador por Nome\n");
        printf(" 5. Buscar Jogadores por Pais\n");
        printf(" 6. Buscar Jogador por Ano de Nascimento\n");
        printf(" 7. Listar Jogadores por Status (Ativo/Aposentado)\n");
        printf("--- Relatorios e Verificacao ---\n");
        printf(" 8. Gerar Relatorio de Titulos\n");
        printf(" 9. Imprimir Estrutura da Arvore B+\n");
        printf(" 10. Sair\n");
        printf("============================================\n");
        printf("Escolha uma opcao: ");

        if (fgets(input_buffer, sizeof(input_buffer), stdin)) { choice = atoi(input_buffer); }
        else { choice = 0; }

        switch (choice) {
            case 1: {
                PlayerData p = {0};
                printf("\n--- Inserindo Novo Jogador ---\nNome: ");
                fgets(p.name, sizeof(p.name), stdin); p.name[strcspn(p.name, "\n")] = 0;
                printf("Sobrenome: ");
                fgets(p.lastname, sizeof(p.lastname), stdin); p.lastname[strcspn(p.lastname, "\n")] = 0;
                printf("Ano de Nascimento: ");
                fgets(input_buffer, sizeof(input_buffer), stdin); p.birth_year = atoi(input_buffer);
                printf("Nacionalidade: ");
                fgets(p.nacionality, sizeof(p.nacionality), stdin); p.nacionality[strcspn(p.nacionality, "\n")] = 0;
                int current_year = 2025;
                p.is_retired = (p.birth_year > 0 && (current_year - p.birth_year) >= 39) ? 1 : 0;

                if (strlen(p.lastname) > 0 && p.birth_year > 1900) {
                    PlayerLocation loc = bpt_insert(fp_index, p);
                    if (loc.leaf_id != -1) {
                        sprintf(name_buffer, "%s %s", p.name, p.lastname);
                        hash_table_insert(player_ht, name_buffer, loc.leaf_id, loc.record_index);
                        inserir_tabela_hash_pais(country_ht, p.nacionality, name_buffer, loc.leaf_id, loc.record_index);
                        printf("-> Jogador '%s' inserido com sucesso.\n", name_buffer);
                    }
                } else { printf("-> Erro: Dados invalidos.\n"); }
                break;
            }
            case 2: {
                printf("\nDigite o nome completo do jogador a ser REMOVIDO: ");
                fgets(name_buffer, sizeof(name_buffer), stdin); name_buffer[strcspn(name_buffer, "\n")] = 0;
                delete_player_by_name(fp_index, player_ht, country_ht, name_buffer);
                break;
            }
            case 3: {
                printf("\nDigite o pais para remover TODOS os seus jogadores: ");
                fgets(input_buffer, sizeof(input_buffer), stdin); input_buffer[strcspn(input_buffer, "\n")] = 0;
                delete_players_by_country(fp_index, player_ht, country_ht, input_buffer);
                break;
            }
            case 4: {
                printf("\nDigite o nome completo do jogador: ");
                fgets(name_buffer, sizeof(name_buffer), stdin); name_buffer[strcspn(name_buffer, "\n")] = 0;
                search_player_by_name(player_ht, name_buffer);
                break;
            }
            case 5: {
                printf("\nDigite o pais: ");
                fgets(input_buffer, sizeof(input_buffer), stdin); input_buffer[strcspn(input_buffer, "\n")] = 0;
                search_players_by_country(country_ht, input_buffer);
                break;
            }
            case 6: {
                int ano_busca = 0;
                printf("\nDigite o ano de nascimento para buscar: ");
                if (fgets(input_buffer, sizeof(input_buffer), stdin)) {
                    ano_busca = atoi(input_buffer);
                }
                if (ano_busca > 0) {
                    rewind(fp_index);
                    PlayerData* jogador_encontrado = bpt_search(fp_index, ano_busca);
                    if (jogador_encontrado != NULL) {
                        printf("-> Jogador encontrado: ");
                        print_player(jogador_encontrado);
                        free(jogador_encontrado);
                    } else {
                        printf("-> Nenhum jogador encontrado para o ano %d.\n", ano_busca);
                    }
                } else { printf("Ano invalido.\n"); }
                break;
            }
            case 7: {
                 printf("\nListando jogadores por status...\n");
                 rewind(fp_index);
                 print_players_by_status(fp_index, 1); // Aposentados
                 printf("\n");
                 rewind(fp_index);
                 print_players_by_status(fp_index, 0); // Ativos
                 break;
            }
            case 8: {
                printf("\nGerando Relatorio Principal de Titulos...\n");
                rewind(fp_index);
                report_titles_by_status(fp_index, "champions.txt");
                break;
            }
            case 9: {
                printf("\nImprimindo estrutura da Arvore B+...\n");
                rewind(fp_index);
                BPTHeader *header = read_header(fp_index);
                if (header) {
                    print_header(header);
                    printf("\n--- Nos Internos ---\n");
                    print_internal_nodes(fp_index);
                    printf("\n--- Nos Folha ---\n");
                    print_leafs(header);
                    free(header);
                }
                break;
            }
            case 10: { printf("Encerrando...\n"); break; }
            default: printf("Opcao invalida. Tente novamente.\n"); break;
        }
    } while (choice != 10);
}
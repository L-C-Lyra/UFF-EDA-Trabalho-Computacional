#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <direct.h> // Para _mkdir no Windows
#else
#include <sys/stat.h> // Para mkdir no Linux/macOS
#endif

#include "bplus_tree.h"
#include "bplus_tree_io.h"
#include "data_parser.h"
#include "hash_table.h"
#include "tabela_hash_pais.h"
#include "hash_table_year.h"
#include "title_manager.h"
#include "operations.h"
#include "ranking_manager.h"
#include "underdog_manager.h"

void show_menu(FILE* fp_index, HashTable* player_ht, TabelaHashPais* country_ht, HashTableYear *year_ht);

void criar_diretorio_se_nao_existir(const char *nome_pasta) {
    #if defined(_WIN32)
        _mkdir(nome_pasta);
    #else
        mkdir(nome_pasta, 0777); // 0777 são as permissões no Linux/macOS
    #endif
}

int main() {
    printf("=======================================================\n");
    printf("        INICIALIZANDO TRABALHO COMPUTACIONAL\n");
    printf("=======================================================\n\n");

    // Passo 0: Cria o diretório 'folhas' automaticamente
    printf("-> Verificando/Criando diretorio para as folhas...\n");
    criar_diretorio_se_nao_existir("folhas");

    // Passo 1: Cria (ou zera) o arquivo de índice e inicializa as estruturas em memória
    FILE *fp_index = fopen("indices_t.bin", "wb+");
    if (fp_index == NULL) { perror("Erro critico ao criar/abrir 'indices_t.bin'"); return 1; }
    init_BPT(fp_index, BTREE_ORDER);

    HashTable* player_ht = create_hash_table();
    TabelaHashPais* country_ht = criar_tabela_hash_pais();
    HashTableYear *year_ht = create_hash_table_year();

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
    fgets(line, sizeof(line), fp_players);
    while (fgets(line, sizeof(line), fp_players) != NULL && player_count < 310) {
        if (line[0] == '[' || line[0] == '\n' || strncmp(line, "Ano", 3) == 0) continue;

        players[player_count] = parse_player_data(line);
        player_count++;
    }
    fclose(fp_players);
    printf("Total de %d jogadores parseados.\n", player_count);


    // Passo 3: Insere cada jogador do array na Árvore B+ e nas Tabelas Hash
    printf("\n--- Iniciando Insercao nas Estruturas de Dados ---\n");
    int inserted_count = 0;
    for (int i = 0; i < player_count; i++) {
        PlayerData p = players[i];

        if (strlen(p.name) > 0 || strlen(p.lastname) > 0) {
            PlayerLocation loc = bpt_insert(fp_index, p);
            if (loc.record_index != -1) {
                char full_name[FULL_NAME_SIZE];
                sprintf(full_name, "%s %s", p.name, p.lastname);
                hash_table_insert(player_ht, full_name, loc.leaf_id, loc.record_index);
                inserir_tabela_hash_pais(country_ht, p.nacionality, full_name, loc.leaf_id, loc.record_index);
                insert_hash_table_year(year_ht, p.birth_year, p.nacionality, loc.leaf_id, loc.record_index);
                inserted_count++;
            }
        }
    }
    printf("-> Estruturas de dados construidas com %d jogadores.\n", inserted_count);

    // Passo 4: Exibe o menu de operações
    show_menu(fp_index, player_ht, country_ht, year_ht);

    // Passo 5: Finalização
    printf("\n=======================================================\n");
    printf("                 EXECUCAO FINALIZADA\n");
    printf("=======================================================\n");

    fclose(fp_index);
    free_hash_table(player_ht);
    liberar_tabela_hash_pais(country_ht);
    free_hash_table_year(year_ht);
    return 0;
}

void show_menu(FILE *fp_index, HashTable* player_ht, TabelaHashPais* country_ht, HashTableYear *year_ht) {
    int choice = 0;
    char input_buffer[100];
    char name_buffer[FULL_NAME_SIZE];

    do {
        printf("\n\n==================== MENU DE OPCOES ====================\n");
        printf("--- Buscas ---\n");
        printf(" 1. Buscar Jogador por Nome\n");
        printf(" 2. Buscar Jogadores por Pais\n");
        printf(" 3. Buscar Jogador EM ATIVIDADE por Nome\n");
        printf(" 4. Buscar Jogadores EM ATIVIDADE por Pais\n");
        printf(" 5. Buscar Jogador APOSENTADO por Nome\n");
        printf(" 6. Buscar Jogadores APOSENTADOS por Pais\n");
        printf("\n--- Insercao e Remocao ---\n");
        printf(" 7. Inserir Novo Jogador\n");
        printf(" 8. Remover Jogador por Nome\n");
        printf(" 9. Remover TODOS os Jogadores de um Pais\n");
        printf(" 10. Remover Jogadores EM ATIVIDADE de um Pais\n");
        printf(" 11. Remover Jogadores APOSENTADOS de um Pais\n");
        printf("\n--- Relatorios e Listagem ---\n");
        printf(" 12. Listar Jogadores por Status (Ativo/Aposentado)\n");
        printf(" 13. Gerar Relatorio de Titulos (Geral)\n");
        printf(" 14. Gerar Relatorio: Participantes do ATP Finals por Ano\n");
        printf(" 15. Gerar Relatorio: Maiores Vencedores por Torneio\n");
        printf(" 16. Imprimir Estrutura da Arvore B+\n");
        printf(" 17. Gerar Relatorio: Jogadores que 'Furaram o Ranking'\n");
        printf(" 18. Funcao de busca 2'\n");
        printf("\n--------------------------------------------------------\n");
        printf(" 0. Sair\n");
        printf("========================================================\n");
        printf("Escolha uma opcao: ");

        if (fgets(input_buffer, sizeof(input_buffer), stdin)) {
            choice = atoi(input_buffer);
        } else {
            choice = 0;
        }

        switch (choice) {
            // --- Buscas ---
            // --- Sair ---
            case 0:
                printf("Encerrando...\n");
                break;
            case 1:
                printf("\nDigite o nome completo do jogador: ");
                fgets(name_buffer, sizeof(name_buffer), stdin); name_buffer[strcspn(name_buffer, "\n")] = 0;
                search_player_by_name(player_ht, name_buffer);
                break;
            case 2:
                printf("\nDigite o pais: ");
                fgets(input_buffer, sizeof(input_buffer), stdin); input_buffer[strcspn(input_buffer, "\n")] = 0;
                search_players_by_country(fp_index, country_ht, input_buffer);
                break;
            case 3:
                printf("\nDigite o nome completo do jogador em atividade: ");
                fgets(name_buffer, sizeof(name_buffer), stdin); name_buffer[strcspn(name_buffer, "\n")] = 0;
                search_active_player_by_name(player_ht, name_buffer);
                break;
            case 4:
                printf("\nDigite o pais para buscar jogadores em atividade: ");
                fgets(input_buffer, sizeof(input_buffer), stdin); input_buffer[strcspn(input_buffer, "\n")] = 0;
                search_active_players_by_country(fp_index, country_ht, input_buffer);
                break;
            case 5:
                printf("\nDigite o nome completo do jogador aposentado: ");
                fgets(name_buffer, sizeof(name_buffer), stdin); name_buffer[strcspn(name_buffer, "\n")] = 0;
                search_retired_player_by_name(player_ht, name_buffer);
                break;
            case 6:
                printf("\nDigite o pais para buscar jogadores aposentados: ");
                fgets(input_buffer, sizeof(input_buffer), stdin); input_buffer[strcspn(input_buffer, "\n")] = 0;
                search_retired_players_by_country(fp_index, country_ht, input_buffer);
                break;

            // --- Inserção e Remoção ---
            case 7: {
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
                PlayerLocation loc = bpt_insert(fp_index, p);
                if (loc.leaf_id != -1) {
                    sprintf(name_buffer, "%s %s", p.name, p.lastname);
                    hash_table_insert(player_ht, name_buffer, loc.leaf_id, loc.record_index);
                    inserir_tabela_hash_pais(country_ht, p.nacionality, name_buffer, loc.leaf_id, loc.record_index);
                    printf("-> Jogador '%s' inserido com sucesso.\n", name_buffer);
                }
                break;
            }
            case 8:
                printf("\nDigite o nome completo do jogador a ser REMOVIDO: ");
                fgets(name_buffer, sizeof(name_buffer), stdin); name_buffer[strcspn(name_buffer, "\n")] = 0;
                delete_player_by_name(fp_index, player_ht, country_ht, name_buffer);
                break;
            case 9:
                printf("\nDigite o pais para remover TODOS os seus jogadores: ");
                fgets(input_buffer, sizeof(input_buffer), stdin); input_buffer[strcspn(input_buffer, "\n")] = 0;
                delete_players_by_country(fp_index, player_ht, country_ht, input_buffer);
                break;
            case 10:
                printf("\nDigite o pais para remover seus jogadores EM ATIVIDADE: ");
                fgets(input_buffer, sizeof(input_buffer), stdin); input_buffer[strcspn(input_buffer, "\n")] = 0;
                delete_active_players_by_country(fp_index, player_ht, country_ht, input_buffer);
                break;
            case 11:
                printf("\nDigite o pais para remover seus jogadores APOSENTADOS: ");
                fgets(input_buffer, sizeof(input_buffer), stdin); input_buffer[strcspn(input_buffer, "\n")] = 0;
                delete_retired_players_by_country(fp_index, player_ht, country_ht, input_buffer);
                break;

            // --- Relatórios e Listagem ---
            case 12:
                 printf("\nListando jogadores por status...\n");
                 rewind(fp_index);
                 print_players_by_status(fp_index, 1);
                 printf("\n");
                 rewind(fp_index);
                 print_players_by_status(fp_index, 0);
                 break;
            case 13:
                printf("\nGerando Relatorio de Titulos (Geral)...\n");
                rewind(fp_index);
                report_titles_by_status(fp_index, "champions.txt");
                break;
            case 14:
                report_atp_finals_participants_by_year("champions.txt");
                break;
            case 15:
                report_tournament_winners("champions.txt");
                break;
            case 16:
                printf("\nImprimindo estrutura da Arvore B+...\n");
                rewind(fp_index);
                BPTHeader *header = read_header(fp_index);
                if (header) {
                    print_header(header);
                    printf("\n--- Nos Internos ---\n");
                    print_internal_nodes(fp_index);
                    printf("\n--- Nos Folha (do diretorio 'folhas/') ---\n");
                    print_leafs(header);
                    free(header);
                }
                break;
            case 17:
                report_underdog_winners("tennis_players.txt", "champions.txt");
                break;
            case 18:
                find_compatriot_slam_birth_year(year_ht, player_ht);
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
                break;
        }
    } while (choice != 0);
}
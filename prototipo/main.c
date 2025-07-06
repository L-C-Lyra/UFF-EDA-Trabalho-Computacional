#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bplus_tree_io.h"
#include "bplus_tree.h"
#include "data_parser.h"
#include "title_manager.h"

void show_menu(FILE *fp_leitura);

int main() {
    printf("=======================================================\n");
    printf("        INICIALIZANDO TRABALHO COMPUTACIONAL\n");
    printf("=======================================================\n\n");

    // --- PASSO 1: CONSTRUÇÃO DA ÁRVORE B+ ---
    printf("[PASSO 1] Construindo a Arvore B+ a partir de 'tennis_players.txt'...\n");

    FILE *fp_index = fopen("indices_t.bin", "wb");
    if (fp_index == NULL) {
        perror("Erro critico ao criar 'indices_t.bin'");
        return 1;
    }
    init_BPT(fp_index, BTREE_ORDER);
    fclose(fp_index);

    fp_index = fopen("indices_t.bin", "r+b");
    if (fp_index == NULL) {
        perror("Erro critico ao reabrir 'indices_t.bin'");
        return 1;
    }

    FILE *fp_players = fopen("tennis_players.txt", "r");
    if (fp_players == NULL) {
        perror("Erro ao abrir 'tennis_players.txt'");
        fclose(fp_index);
        return 1;
    }

    char line[512];
    int player_count = 0;
    fgets(line, sizeof(line), fp_players);

    while (fgets(line, sizeof(line), fp_players) != NULL) {
        if (line[0] == '[' || line[0] == '\n' || strlen(line) < 10) continue;

        PlayerData p = parse_player_data(line);
        if (p.birth_year > 0) {
            bpt_insert(fp_index, p);
            player_count++;
        }
    }
    printf("-> Arvore B+ construida com %d jogadores.\n", player_count);
    fclose(fp_players);
    fclose(fp_index);


    // --- PASSO 2: INICIAR O MODO INTERATIVO ---
    printf("\n...Abrindo ficheiro de indice para modo de leitura e consultas...\n");
    FILE *fp_leitura = fopen("indices_t.bin", "rb");
    if (fp_leitura == NULL) {
        perror("Erro ao abrir 'indices_t.bin' para leitura");
        return 1;
    }

    show_menu(fp_leitura);


    // --- FINALIZAÇÃO ---
    printf("\n=======================================================\n");
    printf("                 EXECUCAO FINALIZADA\n");
    printf("=======================================================\n");

    fclose(fp_leitura);
    return 0;
}

void show_menu(FILE *fp_leitura) {
    int choice = 0;
    do {
        printf("\n\n---------- MENU DE OPCOES ----------\n");
        printf("1. Buscar Jogador por Ano de Nascimento\n");
        printf("2. Gerar Relatorio de Titulos por Status\n");
        printf("3. Listar Jogadores por Status (Ativo/Aposentado)\n");
        printf("4. Imprimir Estrutura da Arvore B+\n");
        printf("5. Sair\n");
        printf("------------------------------------\n");
        printf("Escolha uma opcao: ");

        // Lê a entrada do utilizador de forma segura
        char input_buffer[10];
        if (fgets(input_buffer, sizeof(input_buffer), stdin)) {
            choice = atoi(input_buffer);
        } else {
            choice = 0;
        }

        switch (choice) {
            case 1: {
                int ano_busca = 0;
                printf("\nDigite o ano de nascimento para buscar: ");
                if (fgets(input_buffer, sizeof(input_buffer), stdin)) {
                    ano_busca = atoi(input_buffer);
                }

                if (ano_busca > 0) {
                    rewind(fp_leitura);
                    PlayerData* jogador_encontrado = bpt_search(fp_leitura, ano_busca);
                    if (jogador_encontrado != NULL) {
                        printf("-> Jogador encontrado: ");
                        print_player(jogador_encontrado);
                        free(jogador_encontrado);
                    } else {
                        printf("-> Nenhum jogador encontrado para o ano %d.\n", ano_busca);
                    }
                } else {
                    printf("Ano invalido.\n");
                }
                break;
            }
            case 2: {
                printf("\nGerando Relatorio Principal de Titulos...\n");
                rewind(fp_leitura);
                report_titles_by_status(fp_leitura, "champions.txt");
                break;
            }
            case 3: {
                 printf("\nListando jogadores por status...\n");
                 rewind(fp_leitura);
                 print_players_by_status(fp_leitura, 1); // Aposentados
                 printf("\n");
                 rewind(fp_leitura);
                 print_players_by_status(fp_leitura, 0); // Ativos
                 break;
            }
            case 4: {
                printf("\nVerificando Estado Final da Arvore B+...\n");
                rewind(fp_leitura);
                BPTHeader *header = read_header(fp_leitura);
                if (header) {
                    print_header(header);
                    printf("\n--- Nos Internos ---\n");
                    print_internal_nodes(fp_leitura);
                    printf("\n--- Nos Folha (arquivos separados) ---\n");
                    print_leafs(header);
                    free(header);
                }
                break;
            }
            case 5: {
                printf("Encerrando programa...\n");
                break;
            }
            default: {
                printf("Opcao invalida. Por favor, tente novamente.\n");
                break;
            }
        }

    } while (choice != 5);
}
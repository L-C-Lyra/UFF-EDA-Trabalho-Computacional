#include "bplus_tree_io.h" // Inclui as definições das estruturas da B+ Tree e protótipos de E/S
#include "data_parser.h"   // Inclui o protótipo da função de parsing de dados de jogador
#include <stdio.h>         // Para operações de arquivo (fopen, fclose, printf, perror, fgets)
#include <stdlib.h>        // Para alocação de memória (malloc, free)

int main() {
    // --- 1. Inicialização da Árvore B+ ---
    // Abre o arquivo de índice principal para escrita binária (cria ou sobrescreve)
    FILE *fp_index = fopen("indices_t.bin", "wb");
    if (fp_index == NULL) {
        perror("Erro ao abrir indices_t.bin para escrita");
        return 1;
    }
    // Inicializa o cabeçalho da B+ Tree no arquivo de índice
    init_BPT(fp_index);
    fclose(fp_index); // Fecha o arquivo após a inicialização

    // Reabre o arquivo de índice no modo de leitura binária para verificar o cabeçalho
    BPTHeader *file_header = (BPTHeader *)malloc(sizeof(BPTHeader));
    if (file_header == NULL) {
        perror("Erro ao alocar file_header");
        return 1;
    }
    LeafNode *leaf_node = (LeafNode *)malloc(sizeof(LeafNode));
    if (leaf_node == NULL) {
        perror("Erro ao alocar leaf_node");
        free(file_header);
        return 1;
    }

    fp_index = fopen("indices_t.bin", "rb");
    if (fp_index == NULL) {
        perror("Erro ao reabrir indices_t.bin para leitura");
        free(file_header);
        free(leaf_node);
        return 1;
    }

    // Lê e imprime o cabeçalho da árvore para verificação
    read_header(fp_index, file_header);
    print_header(file_header);

    printf("\n");
    // Imprime informações sobre os nós internos (deve estar vazio na inicialização)
    print_internal_nodes(fp_index);

    // Lê e imprime o primeiro nó folha (a raiz, que está vazia no início)
    // O ID -1 corresponde à primeira folha criada por init_BPT, que é folha_001.bin
    read_leaf_node(-1, leaf_node);
    print_leaf_aux(leaf_node);

    fclose(fp_index); // Fecha o arquivo de índice após as verificações

    free(file_header);
    free(leaf_node);

    // --- 2. Carregamento e Parsing dos Dados de Jogadores ---
    printf("\n--- Carregando e Parseando Dados de tennis_players.txt ---\n");

    FILE *fp_players = fopen("tennis_players.txt", "r");
    if (fp_players == NULL) {
        perror("Erro ao abrir tennis_players.txt");
        return 1;
    }

    char line[512]; // Buffer para armazenar cada linha lida do arquivo
    int line_count = 0;

    // Pular a linha do cabeçalho do arquivo .txt
    if (fgets(line, sizeof(line), fp_players) != NULL) {
        printf("Cabeçalho do arquivo de jogadores ignorado: %s", line);
    } else {
        printf("Arquivo tennis_players.txt está vazio ou erro na leitura do cabeçalho.\n");
        fclose(fp_players);
        return 1;
    }

    // Loop para ler e parsear cada linha de dados de jogador
    printf("Dados de Jogadores Parseados:\n");
    while (fgets(line, sizeof(line), fp_players) != NULL) {
        line_count++;
        // Ignorar linhas de referência de origem (ex: )
        if (line[0] == '[') {
            printf("Ignorando linha de source: %s", line);
            continue;
        }

        // Chama a função de parsing para preencher a estrutura PlayerData
        PlayerData p = parse_player_data(line);

        // --- AQUI É ONDE VOCÊ INTEGRARÁ A INSERÇÃO NA B+ TREE FUTURAMENTE ---
        // Por enquanto, apenas imprime para verificar o parsing
        printf("  Linha %d - Nome: %s %s, Nasc: %d, Rank: %d\n",
               line_count, p.name, p.lastname, p.birth_year, p.best_rank);

        // Exemplo de como seria a chamada de inserção (ainda não implementada)
        // insert_player_into_bpt(fp_index, &p); // fp_index precisaria ser reaberto no modo "rb+" ou "wb+"
        // Se você fosse inserir aqui, precisaria abrir o arquivo de índice no modo correto (rb+ ou ab+) e passá-lo para a função de inserção.
        // A lógica de abrir/fechar o arquivo de índice depende de como você projetar a função de inserção.
        // Pode ser melhor passar o nome do arquivo para as funções de B+Tree e elas mesmas abrirem/fecharem, ou mantê-lo aberto durante a execução do programa.
        // Para este teste, não vamos inserir nada ainda.

        // Opcional: Para testar apenas as primeiras N linhas, descomente a linha abaixo
        // if (line_count >= 10) break;
    }

    fclose(fp_players); // Fecha o arquivo de dados dos jogadores

    printf("\n--- Carregamento e Parsing Concluídos. Próximo passo: Implementar a Inserção na B+ Tree ---\n");

    // --- 3. Menu Interativo (Futuro) ---
    // Esta seção será o loop principal do menu interativo do seu SGBD.
    // Ele permitirá ao usuário consultar, inserir, etc., informações.
    // Exemplo:
    // int choice;
    // do {
    //     printf("\nMenu:\n");
    //     printf("1. Inserir Jogador\n");
    //     printf("2. Buscar Jogador\n");
    //     printf("3. Listar Todos os Jogadores\n");
    //     printf("0. Sair\n");
    //     printf("Escolha uma opção: ");
    //     scanf("%d", &choice);
    //
    //     switch (choice) {
    //         case 1: // Chamar função de inserção
    //             break;
    //         case 2: // Chamar função de busca
    //             break;
    //         case 3: // Chamar função de listagem
    //             break;
    //         case 0:
    //             printf("Saindo...\n");
    //             break;
    //         default:
    //             printf("Opção inválida.\n");
    //     }
    // } while (choice != 0);

    return 0;
}
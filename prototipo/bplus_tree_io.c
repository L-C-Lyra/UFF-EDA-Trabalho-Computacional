#include "bplus_tree_io.h"

void write_header(FILE *index_file, BPTHeader *header) {
    fseek(index_file, 0, SEEK_SET);
    fwrite(header, sizeof(BPTHeader), 1, index_file);
}

BPTHeader *read_header(FILE *index_file) {
    BPTHeader *header = (BPTHeader *)malloc(sizeof(BPTHeader));
    if (!header) {
        perror("Falha ao alocar memoria para o header");
        return NULL;
    }

    fseek(index_file, 0, SEEK_SET);
    if (fread(header, sizeof(BPTHeader), 1, index_file) != 1) {
        free(header);
        return NULL;
    }
    return header;
}

void write_internal_node(FILE *index_file, int node_offset, InternalNode *node) {
    fseek(index_file, node_offset, SEEK_SET);
    fwrite(node, sizeof(InternalNode), 1, index_file);
}

InternalNode *read_internal_node(FILE *index_file, int node_offset) {
    InternalNode *node = (InternalNode *)malloc(sizeof(InternalNode));
    if (!node) {
        perror("Falha ao alocar memoria para no interno");
        return NULL;
    }

    fseek(index_file, node_offset, SEEK_SET);
    if(fread(node, sizeof(InternalNode), 1, index_file) != 1) {
        free(node);
        return NULL;
    }
    return node;
}

void write_leaf_node(int leaf_id, LeafNode *node) {
    char filename[FILENAME_SIZE];
    sprintf(filename, "folha_%03d.bin", (-leaf_id));
    FILE *leaf_file = fopen(filename, "wb");
    if (leaf_file == NULL) {
        perror("Falha ao abrir arquivo folha");
        return;
    }
    fwrite(node, sizeof(LeafNode), 1, leaf_file);
    fclose(leaf_file);
}

LeafNode *read_leaf_node(int leaf_id) {
    LeafNode *node = (LeafNode *)malloc(sizeof(LeafNode));
    if (!node) {
        perror("Falha ao alocar memoria para no folha");
        return NULL;
    }

    char filename[FILENAME_SIZE];
    sprintf(filename, "folha_%03d.bin", (-leaf_id));
    FILE *leaf_file = fopen(filename, "rb");

    if (leaf_file == NULL) {
        free(node);
        perror("Falha ao abrir arquivo folha para leitura");
        return NULL;
    }

    if(fread(node, sizeof(LeafNode), 1, leaf_file) != 1){
        fclose(leaf_file);
        free(node);
        return NULL;
    }

    fclose(leaf_file);
    return node;
}

int init_BPT(FILE *index_file, const int order) {
    BPTHeader init_header = {order, -1, 0, 0, sizeof(BPTHeader), -1};
    write_header(index_file, &init_header);
}


/*
-----------------------------------------------------
        FUNÇÕES QUE NÃO FARÃO PARTE DA API
-----------------------------------------------------
*/

void write_internal_node_from_id(FILE *index_file, int node_id, InternalNode *node) {
    int offset = sizeof(BPTHeader) + (node_id * sizeof(InternalNode));
    fseek(index_file, offset, SEEK_SET);
    fwrite(node, sizeof(InternalNode), 1, index_file);
}

InternalNode *read_internal_node_from_id(FILE *index_file, int node_id) {
    int offset = sizeof(BPTHeader) + (node_id * sizeof(InternalNode));

    InternalNode *node = (InternalNode *)malloc(sizeof(InternalNode));
    fseek(index_file, offset, SEEK_SET);
    if(fread(node, sizeof(InternalNode), 1, index_file)) {
        return node;
    }
    return NULL;
}

void add_leaf_node(FILE *index_file, LeafNode *leafnode) {
    BPTHeader *header = read_header(index_file);
    if (!header) {
        printf("Erro ao ler header para adicionar no folha.\n");
        return;
    }

    char filename[FILENAME_SIZE];
    sprintf(filename, "folha_%03d.bin", header->next_leaf_id);
    FILE *leaf_file = fopen(filename, "wb");
    if (!leaf_file) {
        printf("Erro ao abrir arquivo folha\n");
        free(header);
        return;
    }

    leafnode->next_leaf_id = header->next_leaf_id + 1;
    fwrite(leafnode, sizeof(LeafNode), 1, leaf_file);
    fclose(leaf_file);

    header->leaf_count++;
    header->next_leaf_id++;
    write_header(index_file, header);

    free(header);
}

void add_internal_node(FILE *index_file, InternalNode *node) {
    BPTHeader *t_header = read_header(index_file);
    if (!t_header) {
        printf("Erro ao ler header para adicionar no interno.\n");
        return;
    }

    fseek(index_file, t_header->next_free_offset, SEEK_SET);
    fwrite(node, sizeof(InternalNode), 1, index_file);

    t_header->internal_node_count++;
    t_header->next_free_offset += sizeof(InternalNode);
    write_header(index_file, t_header);

    free(t_header);
}


void print_header(BPTHeader *header) {

    printf("<Header>\n");
    printf("Order: %d\n", header->order);
    printf("Root node offset: %d\n", header->root_offset);
    printf("Internal node count %d\n", header->internal_node_count);
    printf("Leaf file count: %d\n", header->leaf_count);
    printf("Next free offset: %d\n", header->next_free_offset);
    printf("Next leaf id: %d\n", header->next_leaf_id);
    printf("\n");
}

void print_node_aux(InternalNode *node) {
    if (!node) return;
    printf("<Node>\n");
    printf("Num keys: %d\n", node->num_keys);
    printf("Keys: ");
    int i;
    for (i = 0; i < node->num_keys; i++) {
        printf("%d ", node->keys[i]);
    }
    printf("\n");
    printf("Children pointers: ");
    for (i = 0; i <= node->num_keys; i++) {
        printf("%d ", node->children_pointers[i]);
    }
    printf("\n\n");
}

void print_internal_nodes(FILE *index_file) {
    BPTHeader *header = read_header(index_file);
    if (!header) {
        printf("Falha ao ler o header do arquivo de indices.\n");
        return;
    }

    if (header->internal_node_count == 0) {
        printf("Arquivo de indices vazio\n");
        free(header);
        return;
    }

    fseek(index_file, header->root_offset, SEEK_SET);
    int start = ftell(index_file);
    InternalNode *temp_node;

    for (int j = 0; j < header->internal_node_count; j++) {
        temp_node = read_internal_node(index_file, (start + j * sizeof(InternalNode)));
        if (temp_node != NULL) {
            print_node_aux(temp_node);
            free(temp_node);
        }
    }

    free(header);
}

void print_player(PlayerData *record) {
    printf("Player -> Nome: %s %s, Ano Nasc: %d, Rank: %d\n",
        record->name, 
        record->lastname,
        record->birth_year, 
        record->best_rank);
}

void print_players_by_status(FILE *index_file, int is_retired_status) {
    BPTHeader *header = read_header(index_file);
    if (!header || header->leaf_count == 0) {
        printf("A arvore esta vazia ou o header nao pode ser lido.\n");
        if(header) free(header);
        return;
    }

    int current_leaf_id = -1;

    printf("\n--- Listando Jogadores com Status: %s ---\n", is_retired_status == 1 ? "Aposentado" : "Ativo");

    while (current_leaf_id != 0) {
        LeafNode *leaf = read_leaf_node(current_leaf_id);
        if (!leaf) {
            printf("Erro ao ler a folha com ID: %d\n", current_leaf_id);
            break;
        }

        for (int i = 0; i < leaf->num_records; i++) {
            if (leaf->records[i].is_retired == is_retired_status) {
                print_player(&leaf->records[i]);
            }
        }

        current_leaf_id = leaf->next_leaf_id;
        free(leaf);
    }

    free(header);
}

void print_leaf_aux(LeafNode *node) {
    if (!node) return;
    printf("------------------------------------------\n");
    printf("Numero de Registros: %d\n", node->num_records);
    printf("ID da Proxima Folha: %d\n", node->next_leaf_id);
    printf("\nRegistros:\n");

    if (node->num_records == 0) {
        printf("  (Folha vazia)\n");
    } 
    else {
        for (int i = 0; i < node->num_records; i++) {
            printf("  [%d] -> Nome: %s %s, Ano Nasc: %d, Rank: %d\n", 
                   i, 
                   node->records[i].name, 
                   node->records[i].lastname,
                   node->records[i].birth_year, 
                   node->records[i].best_rank);
        }
    }
    printf("------------------------------------------\n\n");
}

void print_leafs(BPTHeader *header) {
    if (header == NULL || header->leaf_count == 0) {
        printf("Arvore vazia.\n");
        return;
    }

    LeafNode *node = NULL;
    for (int i = 1; i <= header->leaf_count; i++) {
        node = read_leaf_node(-i);
        if (node != NULL) {
            print_leaf_aux(node);
            free(node);
        }
    }
}


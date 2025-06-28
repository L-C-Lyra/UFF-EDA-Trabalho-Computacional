#include "bplus_tree_io.h"

void write_header(FILE *index_file, BPTHeader *header) {
    fseek(index_file, 0, SEEK_SET);
    fwrite(header, sizeof(BPTHeader), 1, index_file);
}

void read_header(FILE *index_file, BPTHeader *header) {
    fseek(index_file, 0, SEEK_SET);
    fread(header, sizeof(BPTHeader), 1, index_file);
}

void write_internal_node(FILE *index_file, int offset, InternalNode *node) {
    fseek(index_file, offset, SEEK_SET);
    fwrite(node, sizeof(InternalNode), 1, index_file);
}

void read_internal_node(FILE *index_file, int offset, InternalNode *node) {
    fseek(index_file, offset, SEEK_SET);
    fread(node, sizeof(InternalNode), 1, index_file);
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

void read_leaf_node(int leaf_id, LeafNode *node) {
    char filename[FILENAME_SIZE];
    sprintf(filename, "folha_%03d.bin", (-leaf_id));
    FILE *leaf_file = fopen(filename, "rb");
    if (leaf_file == NULL) {
        perror("Falha ao abrir arquivo folha");
        return;
    }
    fread(node, sizeof(LeafNode), 1, leaf_file);
    fclose(leaf_file);
}

int init_BPT(FILE *index_file) {
    BPTHeader init_header = {BTREE_ORDER, -1, 0, 0, sizeof(BPTHeader), -1};
    write_header(index_file, &init_header);
}


/*
-----------------------------------------------------
        FUNÇÕES QUE NÃO FARÃO PARTE DA API
-----------------------------------------------------
*/


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

void add_leaf_node(FILE *index_file, LeafNode *leafnode) {
    BPTHeader header;
    read_header(index_file, &header);

    char filename[FILENAME_SIZE];
    sprintf(filename, "folha_%03d.bin", header.next_leaf_id);
    FILE *leaf_file = fopen(filename, "wb");
    if (!leaf_file) {
        printf("Erro ao abrir arquivo folha\n");
    }

    leafnode->next_leaf_id = header.next_leaf_id + 1;
    fwrite(leafnode, sizeof(LeafNode), 1, leaf_file);
    fclose(leaf_file);

    header.leaf_count++;
    header.next_leaf_id++;
    write_header(index_file, &header);
}

void add_internal_node(FILE *index_file, InternalNode *node) {
    BPTHeader t_header;
    read_header(index_file, &t_header);

    fseek(index_file, t_header.next_free_offset, SEEK_SET);
    fwrite(node, sizeof(InternalNode), 1, index_file);

    // atualiza contagem do numero de nos
    t_header.internal_node_count++;
    t_header.next_free_offset += sizeof(InternalNode);
    write_header(index_file, &t_header);
}

void print_node_aux(InternalNode *node) {
    printf("<Node>\n");
    printf("Num keys: %d\n", node->num_keys);
    printf("Keys: ");
    int i;
    for (i = 0; i < node->num_keys; i++) {
        printf("'%s' ", node->keys[i]);
    }
    printf("\n");
    printf("Children pointers: ");
    for (i = 0; i <= node->num_keys; i++) {
        printf("%d ", node->children_pointers[i]);
    }
    printf("\n");
    printf("Is pointer to leaf: %d\n", node->is_pointer_to_leaf);
    printf("\n");
}

void print_internal_nodes(FILE *index_file) {
    BPTHeader header;
    InternalNode temp_node;
    read_header(index_file, &header);
    if (header.internal_node_count == 0) {
        printf("Arquivo de indices vazio\n");
        return;
    }

    fseek(index_file, header.root_offset, SEEK_SET);
    int start = ftell(index_file);
    for (int j = 0; j < header.internal_node_count; j++) {
        read_internal_node(index_file, (start + j * sizeof(InternalNode)), &temp_node);
        print_node_aux(&temp_node);
    }
}


void print_leaf_aux(LeafNode *node) {

    // --- Impressão Formatada ---
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
    if (header && header->leaf_count == 0) {
        printf("Arvore vazia.\n");
        return;
    }

    LeafNode node;
    for (int i = 1; i <= header->leaf_count; i++) {
        read_leaf_node(i, &node);        
        print_leaf_aux(&node);
    }
}
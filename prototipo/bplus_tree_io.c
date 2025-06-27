#include "bplus_tree_io.h"

void write_header(FILE *index_file, BPTHeader *header) {
    fseek(index_file, 0, SEEK_SET);
    fwrite(header, sizeof(BPTHeader), 1, index_file);
}

void read_header(FILE *index_file, BPTHeader *header) {
    fseek(index_file, 0, SEEK_SET);
    fread(header, sizeof(BPTHeader), 1, index_file);
}

void init_header(FILE *index_file) {
    BPTHeader new_header = {BTREE_ORDER, sizeof(BPTHeader), 0, 0, sizeof(BPTHeader), 0};
    write_header(index_file, &new_header);
}

void print_header(BPTHeader *header) {

    printf("<Header>\n");
    printf("Order: %d\n", header->order);
    printf("Root node offset: %d\n", header->root_node_offset);
    printf("Internal node count %d\n", header->internal_node_count);
    printf("Leaf file count: %d\n", header->leaf_file_count);
    printf("Next free offset: %d\n", header->next_free_offset);
    printf("Next leaf id: %d\n", header->next_leaf_id);
    printf("\n");
}

void write_internal_node(FILE *index_file, int offset, InternalNode *node) {
    fseek(index_file, offset, SEEK_SET);
    fwrite(node, sizeof(InternalNode), 1, index_file);
}

void read_internal_node(FILE *index_file, int offset, InternalNode *node) {
    fseek(index_file, offset, SEEK_SET);
    fread(node, sizeof(InternalNode), 1, index_file);
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
    printf("Is leaf: %d\n", node->is_leaf);
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

    fseek(index_file, header.root_node_offset, SEEK_SET);
    int start = ftell(index_file);
    for (int j = 0; j < header.internal_node_count; j++) {
        read_internal_node(index_file, (start + j * sizeof(InternalNode)), &temp_node);
        print_node_aux(&temp_node);
    }
}

// int main()
// {
//     FILE *fp = fopen("indices.bin", "rb+");
//     init_header(fp);

//     InternalNode node1 = {0, 5, {"Zarias Keith", "John Arias", "Beckman B.", "Roger Federer", "Carlos Alcaraz"}, {4, 5, 6, 12, 3, 10}, 0};
//     InternalNode node2 = {0, 4, {"David Pate", "Scott Davis", "Henri Leconte"}, {0, 1, 5, 7}, 0};
//     InternalNode node3 = {1, 2, {"Gabriel Monteiro", "Bitelo Raquete"}, {2, 8, 9}, 0};

//     add_internal_node(fp, &node1);
//     add_internal_node(fp, &node2);
//     add_internal_node(fp, &node3);


//     fclose(fp);


//     BPTHeader *file_header = (BPTHeader *)malloc(sizeof(BPTHeader));
//     InternalNode *file_node = (InternalNode *)malloc(sizeof(InternalNode));

//     fp = fopen("indices.bin", "rb");

//     read_header(fp, file_header);
//     print_header(file_header);

//     printf("\n");
//     print_internal_nodes(fp);
// }
#include "bplus_tree_io.h"

void write_header(FILE *index_file, BPTHeader *header) {
    fwrite(header, sizeof(BPTHeader), 1, index_file);
}

void read_header(FILE *index_file, BPTHeader *header) {
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

void print_header(BPTHeader *header) {
    printf("<Header>\n");
    printf("Order: %d\n", header->order);
    printf("Root node offset: %d\n", header->root_node_offset);
    printf("Internal node count %d\n", header->internal_node_count);
    printf("Leaf file count: %d\n", header->leaf_file_count);
    printf("Next internal node id: %d\n", header->next_internal_node_id);
    printf("Next leaf id: %d\n", header->next_leaf_id);
}

void print_internal_node(InternalNode *node) {
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
}

int main()
{
    FILE *fp = fopen("indices.bin", "wb");
    BPTHeader new_header = {3, sizeof(BPTHeader), 0, 0, sizeof(BPTHeader), 0};
    InternalNode new_node = {1, 3, {"Zarias Keith", "John Arias", "Beckman B."}, {4, 5, 6}, 0};
    write_header(fp, &new_header);
    write_internal_node(fp, new_header.root_node_offset, &new_node);
    fclose(fp);

    fp = fopen("indices.bin", "rb");
    BPTHeader *file_header = (BPTHeader *)malloc(sizeof(BPTHeader));
    InternalNode *file_node = (InternalNode *)malloc(sizeof(InternalNode));

    read_header(fp, file_header);
    read_internal_node(fp, file_header->root_node_offset, file_node);

    print_header(file_header);
    print_internal_node(file_node);
    fclose(fp);
}
#include "bplus_tree_io.h"


PlayerData *bpt_search(FILE *index_file, char *key) {
    BPTHeader *header = read_header(index_file);

    if (header->leaf_count == 0) {
        return NULL;
    }

    int current_ptr = header->root_offset;

    while (current_ptr >= 0) {
        InternalNode *current_node = read_internal_node(index_file, current_ptr);

        int i = 0;
        // se strcmp(key, current_node->keys[i]) < 0 , key "vem antes" de current_node->keys[i]
        while (i < current_node->num_keys && strcmp(key, current_node->keys[i]) >= 0) {
            i++;
        }

        current_ptr = (current_node->children_pointers[i]);
    }
    free(header);

    LeafNode *leaf = read_leaf_node(current_ptr);
    for (int i = 0; i < leaf->num_records; i++) {
        if (strcmp(key, leaf->records[i].name) == 0) {
            PlayerData *record = (PlayerData *)malloc(sizeof(PlayerData));
            memcpy(record, &leaf->records[i], sizeof(PlayerData));
            free(leaf);
            printf("\nChave encontrada na folha %d\n", (-current_ptr));
            return record;
        }
    }
    free(leaf);
    return NULL;
}


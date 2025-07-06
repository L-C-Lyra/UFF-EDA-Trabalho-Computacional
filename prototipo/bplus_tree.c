#include "bplus_tree.h"
#include "bplus_tree_io.h"

void insert_into_parent(FILE *index_file, int parent_offset, int key, int right_child_pointer);
int find_parent_offset(FILE *index_file, int root_offset, int child_pointer);

int find_parent_offset(FILE *index_file, int root_offset, int child_pointer) {
    if (root_offset == -1 || root_offset == child_pointer) {
        return -1;
    }

    BPTHeader *header = read_header(index_file);
    if (!header) {
        return -1;
    }

    int queue_size = header->internal_node_count + 1;
    int *queue = (int *)malloc(sizeof(int) * queue_size);
    if (!queue) {
        free(header);
        return -1;
    }

    int head = 0, tail = 0;
    queue[tail++] = root_offset;

    int parent_found_offset = -1;

    while(head < tail && parent_found_offset == -1) {
        int current_offset = queue[head++];
        InternalNode* current_node = read_internal_node(index_file, current_offset);
        if(!current_node) continue;

        for (int i = 0; i <= current_node->num_keys; i++) {
            if (current_node->children_pointers[i] == child_pointer) {
                parent_found_offset = current_offset;
                break;
            }
            if(current_node->children_pointers[i] >= 0){
                if (tail < queue_size) {
                    queue[tail++] = current_node->children_pointers[i];
                }
            }
        }
        free(current_node);
    }

    free(queue);
    free(header);
    return parent_found_offset;
}

void insert_into_parent(FILE *index_file, int parent_offset, int key, int right_child_pointer) {
    BPTHeader *header = read_header(index_file);

    if (parent_offset == -1) {
        InternalNode *new_root = (InternalNode*)calloc(1, sizeof(InternalNode));
        int new_root_offset = header->next_free_offset;

        new_root->num_keys = 1;
        new_root->keys[0] = key;
        new_root->children_pointers[0] = header->root_offset;
        new_root->children_pointers[1] = right_child_pointer;

        write_internal_node(index_file, new_root_offset, new_root);

        header->root_offset = new_root_offset;
        header->internal_node_count++;
        header->next_free_offset += sizeof(InternalNode);
        write_header(index_file, header);

        free(new_root);
        free(header);
        return;
    }

    InternalNode *parent = read_internal_node(index_file, parent_offset);
    if(!parent) { free(header); return; }

    if (parent->num_keys < KEY_SIZE) {
        int i = parent->num_keys - 1;
        while (i >= 0 && key < parent->keys[i]) {
            parent->keys[i + 1] = parent->keys[i];
            parent->children_pointers[i + 2] = parent->children_pointers[i + 1];
            i--;
        }
        parent->keys[i + 1] = key;
        parent->children_pointers[i + 2] = right_child_pointer;
        parent->num_keys++;
        write_internal_node(index_file, parent_offset, parent);
        free(parent);
        free(header);
        return;
    }

    int temp_keys[KEY_SIZE + 1];
    int temp_pointers[KEY_SIZE + 2];
    int pos = 0;
    while (pos < KEY_SIZE && key > parent->keys[pos]) pos++;

    memcpy(temp_keys, parent->keys, pos * sizeof(int));
    memcpy(temp_pointers, parent->children_pointers, (pos + 1) * sizeof(int));

    temp_keys[pos] = key;
    temp_pointers[pos + 1] = right_child_pointer;

    memcpy(&temp_keys[pos + 1], &parent->keys[pos], (KEY_SIZE - pos) * sizeof(int));
    memcpy(&temp_pointers[pos + 2], &parent->children_pointers[pos + 1], (KEY_SIZE - pos + 1) * sizeof(int));

    int split_point = KEY_SIZE / 2;
    int promoted_key = temp_keys[split_point];

    InternalNode *new_internal = (InternalNode*)calloc(1, sizeof(InternalNode));
    int new_node_offset = header->next_free_offset;
    header->next_free_offset += sizeof(InternalNode);
    header->internal_node_count++;

    parent->num_keys = split_point;
    memcpy(parent->keys, temp_keys, split_point * sizeof(int));
    memcpy(parent->children_pointers, temp_pointers, (split_point + 1) * sizeof(int));

    new_internal->num_keys = KEY_SIZE - split_point;
    memcpy(new_internal->keys, &temp_keys[split_point + 1], new_internal->num_keys * sizeof(int));
    memcpy(new_internal->children_pointers, &temp_pointers[split_point + 1], (new_internal->num_keys + 1) * sizeof(int));

    write_internal_node(index_file, parent_offset, parent);
    write_internal_node(index_file, new_node_offset, new_internal);

    int grand_parent_offset = find_parent_offset(index_file, header->root_offset, parent_offset);
    write_header(index_file,header);
    insert_into_parent(index_file, grand_parent_offset, promoted_key, new_node_offset);

    free(header);
    free(parent);
    free(new_internal);
}

void insert_into_leaf_and_handle_split(FILE *index_file, int leaf_id, PlayerData record) {
    LeafNode *leaf = read_leaf_node(leaf_id);
    if(!leaf) return;

    if (leaf->num_records < KEY_SIZE) {
        int i = leaf->num_records - 1;
        while (i >= 0 && record.birth_year < leaf->records[i].birth_year) {
            leaf->records[i + 1] = leaf->records[i];
            i--;
        }
        leaf->records[i + 1] = record;
        leaf->num_records++;
        write_leaf_node(leaf_id, leaf);
        free(leaf);
        return;
    }

    BPTHeader *header = read_header(index_file);

    LeafNode *new_leaf = (LeafNode *)calloc(1, sizeof(LeafNode));
    int new_leaf_id = header->next_leaf_id;

    new_leaf->next_leaf_id = leaf->next_leaf_id;
    new_leaf->prev_lead_id = leaf_id;
    leaf->next_leaf_id = new_leaf_id;
    if(new_leaf->next_leaf_id != 0) {
        LeafNode *next_leaf = read_leaf_node(new_leaf->next_leaf_id);
        if(next_leaf){
            next_leaf->prev_lead_id = new_leaf_id;
            write_leaf_node(new_leaf->next_leaf_id, next_leaf);
            free(next_leaf);
        }
    }

    PlayerData temp_records[KEY_SIZE + 1];
    int pos = 0;
    while (pos < KEY_SIZE && record.birth_year > leaf->records[pos].birth_year) pos++;

    memcpy(temp_records, leaf->records, pos * sizeof(PlayerData));
    temp_records[pos] = record;
    memcpy(&temp_records[pos + 1], &leaf->records[pos], (KEY_SIZE - pos) * sizeof(PlayerData));

    int split_point = (KEY_SIZE + 1) / 2;
    leaf->num_records = split_point;
    new_leaf->num_records = (KEY_SIZE + 1) - split_point;

    memcpy(leaf->records, temp_records, split_point * sizeof(PlayerData));
    memcpy(new_leaf->records, &temp_records[split_point], new_leaf->num_records * sizeof(PlayerData));

    write_leaf_node(leaf_id, leaf);
    write_leaf_node(new_leaf_id, new_leaf);

    int promoted_key = new_leaf->records[0].birth_year;

    int parent_offset = find_parent_offset(index_file, header->root_offset, leaf_id);

    header->leaf_count++;
    header->next_leaf_id--;
    write_header(index_file, header);

    insert_into_parent(index_file, parent_offset, promoted_key, new_leaf_id);

    free(header);
    free(leaf);
    free(new_leaf);
}

void bpt_insert(FILE *index_file, PlayerData record) {
    BPTHeader *header = read_header(index_file);
    if(!header) return;

    if (header->root_offset == -1) {
        LeafNode *leaf = (LeafNode *)calloc(1, sizeof(LeafNode));
        leaf->num_records = 1;
        leaf->records[0] = record;
        leaf->next_leaf_id = 0;
        leaf->prev_lead_id = 0;

        int first_leaf_id = -1;
        write_leaf_node(first_leaf_id, leaf);
        free(leaf);

        InternalNode* root = (InternalNode*)calloc(1, sizeof(InternalNode));
        root->num_keys = 0;
        root->children_pointers[0] = first_leaf_id;

        header->root_offset = sizeof(BPTHeader);
        write_internal_node(index_file, header->root_offset, root);

        header->internal_node_count = 1;
        header->leaf_count = 1;
        header->next_free_offset = sizeof(BPTHeader) + sizeof(InternalNode);
        header->next_leaf_id = -2;
        write_header(index_file, header);

        free(root);
        free(header);
        return;
    }

    int current_offset = header->root_offset;
    free(header);

    while (current_offset >= 0) {
        InternalNode *current_node = read_internal_node(index_file, current_offset);
        if(!current_node) return;

        int i = 0;
        while (i < current_node->num_keys && record.birth_year >= current_node->keys[i]) {
            i++;
        }

        int next_offset = current_node->children_pointers[i];
        free(current_node);

        if (next_offset < 0) {
            insert_into_leaf_and_handle_split(index_file, next_offset, record);
            return;
        }
        current_offset = next_offset;
    }
}

PlayerData *bpt_search(FILE *index_file, int birth_year_key) {
    BPTHeader *header = read_header(index_file);
    if (!header || header->leaf_count == 0) {
        if(header) free(header);
        return NULL;
    }

    int current_ptr = header->root_offset;
    free(header);

    while (current_ptr >= 0) {
        InternalNode *current_node = read_internal_node(index_file, current_ptr);
        if (!current_node) {
            return NULL;
        }

        int i = 0;
        while (i < current_node->num_keys && birth_year_key >= current_node->keys[i]) {
            i++;
        }
        current_ptr = current_node->children_pointers[i];
        free(current_node);
    }

    LeafNode *leaf = read_leaf_node(current_ptr);
    if (!leaf) {
        return NULL;
    }

    PlayerData *record = NULL;

    for (int i = 0; i < leaf->num_records; i++) {
        if (birth_year_key == leaf->records[i].birth_year) {
            record = (PlayerData *)malloc(sizeof(PlayerData));
            if (record) {
                memcpy(record, &leaf->records[i], sizeof(PlayerData));
            }
            break;
        }
    }

    free(leaf);
    return record;
}
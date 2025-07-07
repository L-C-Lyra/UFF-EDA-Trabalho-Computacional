#include "bplus_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Protótipos das Funções Estáticas (Privadas ao Módulo) ---
static int find_parent_offset(FILE *index_file, int root_offset, int child_offset);
static int get_node_key_count(FILE* index_file, int offset);
static PlayerLocation insert_into_parent(FILE *index_file, int parent_offset, int key, int left_child_pointer, int right_child_pointer);
static PlayerLocation insert_into_leaf_and_handle_split(FILE *index_file, int leaf_id, PlayerData record);
static void delete_recursive(FILE *index_file, int current_offset, int birth_year_key, const char* full_name);
static void handle_underflow(FILE *index_file, int node_offset, int parent_offset, int child_index_in_parent);
static void case3a_redistribute(FILE* index_file, int node_offset, int sibling_offset, int parent_offset, int child_index, int is_prev_sibling);
static void case3b_merge(FILE* index_file, int node_offset, int sibling_offset, int parent_offset, int child_index);


PlayerLocation bpt_insert(FILE *index_file, PlayerData record) {
    BPTHeader *header = read_header(index_file);
    PlayerLocation loc = {-1, -1};
    if(!header) return loc;

    if (header->root_offset == -1) {
        LeafNode *leaf = (LeafNode *)calloc(1, sizeof(LeafNode));
        leaf->num_records = 1;
        leaf->records[0] = record;
        leaf->next_leaf_id = 0;
        leaf->prev_lead_id = 0;
        int first_leaf_id = -1;
        write_leaf_node(first_leaf_id, leaf);

        loc.leaf_id = first_leaf_id;
        loc.record_index = 0;

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
        return loc;
    }

    int current_offset = header->root_offset;
    free(header);

    while (current_offset >= 0) {
        InternalNode *current_node = read_internal_node(index_file, current_offset);
        if(!current_node) return loc;
        int i = 0;
        while (i < current_node->num_keys && record.birth_year >= current_node->keys[i]) i++;
        int next_offset = current_node->children_pointers[i];
        free(current_node);
        if (next_offset < 0) {
            return insert_into_leaf_and_handle_split(index_file, next_offset, record);
        }
        current_offset = next_offset;
    }
    return loc;
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
        if (!current_node) return NULL;
        int i = 0;
        while (i < current_node->num_keys && birth_year_key >= current_node->keys[i]) i++;
        current_ptr = current_node->children_pointers[i];
        free(current_node);
    }
    LeafNode *leaf = read_leaf_node(current_ptr);
    if (!leaf) return NULL;
    PlayerData *record = NULL;
    for (int i = 0; i < leaf->num_records; i++) {
        if (birth_year_key == leaf->records[i].birth_year) {
            record = (PlayerData *)malloc(sizeof(PlayerData));
            if (record) memcpy(record, &leaf->records[i], sizeof(PlayerData));
            break;
        }
    }
    free(leaf);
    return record;
}

void bpt_delete(FILE* index_file, int birth_year_key, const char* full_name) {
    BPTHeader* header = read_header(index_file);
    if (!header || header->root_offset == -1) {
        if (header) free(header);
        return;
    }

    delete_recursive(index_file, header->root_offset, birth_year_key, full_name);

    InternalNode* root = read_internal_node(index_file, header->root_offset);
    if (root && root->num_keys == 0 && root->children_pointers[0] >= 0) {
        header->root_offset = root->children_pointers[0];
        write_header(index_file, header);
    }

    if(root) free(root);
    free(header);
}

static PlayerLocation insert_into_leaf_and_handle_split(FILE *index_file, int leaf_id, PlayerData record) {
    PlayerLocation loc = {-1, -1};
    LeafNode *leaf = read_leaf_node(leaf_id);
    if(!leaf) return loc;

    if (leaf->num_records < KEY_SIZE) {
        int i = leaf->num_records - 1;
        while (i >= 0 && record.birth_year < leaf->records[i].birth_year) {
            leaf->records[i + 1] = leaf->records[i];
            i--;
        }
        leaf->records[i + 1] = record;
        leaf->num_records++;
        loc.leaf_id = leaf_id;
        loc.record_index = i + 1;
        write_leaf_node(leaf_id, leaf);
        free(leaf);
        return loc;
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

    if (pos <= split_point) {
        loc.leaf_id = leaf_id;
        loc.record_index = pos;
    } else {
        loc.leaf_id = new_leaf_id;
        loc.record_index = pos - split_point;
    }

    write_leaf_node(leaf_id, leaf);
    write_leaf_node(new_leaf_id, new_leaf);
    int promoted_key = new_leaf->records[0].birth_year;
    int parent_offset = find_parent_offset(index_file, header->root_offset, leaf_id);
    header->leaf_count++;
    header->next_leaf_id--;
    write_header(index_file, header);
    insert_into_parent(index_file, parent_offset, promoted_key, leaf_id, new_leaf_id);
    free(header);
    free(leaf);
    free(new_leaf);
    return loc;
}

static PlayerLocation insert_into_parent(FILE *index_file, int parent_offset, int key, int left_child_pointer, int right_child_pointer) {
    PlayerLocation loc = {-1,-1};
    BPTHeader *header = read_header(index_file);
    if (parent_offset == -1) {
        InternalNode *new_root = (InternalNode*)calloc(1, sizeof(InternalNode));
        int new_root_offset = header->next_free_offset;
        new_root->num_keys = 1;
        new_root->keys[0] = key;
        new_root->children_pointers[0] = left_child_pointer;
        new_root->children_pointers[1] = right_child_pointer;
        write_internal_node(index_file, new_root_offset, new_root);
        header->root_offset = new_root_offset;
        header->internal_node_count++;
        header->next_free_offset += sizeof(InternalNode);
        write_header(index_file, header);
        free(new_root);
        free(header);
        return loc;
    }

    InternalNode *parent = read_internal_node(index_file, parent_offset);
    if(!parent) { free(header); return loc; }

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
        return loc;
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
    memcpy(&temp_pointers[pos + 2], &parent->children_pointers[pos + 1], (KEY_SIZE - pos) * sizeof(int));
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
    insert_into_parent(index_file, grand_parent_offset, promoted_key, parent_offset, new_node_offset);
    free(header);
    free(parent);
    free(new_internal);
    return loc;
}

static void delete_recursive(FILE *index_file, int current_offset, int birth_year_key, const char* full_name) {
    if (current_offset < 0) {
        LeafNode* leaf = read_leaf_node(current_offset);
        if (!leaf) return;
        int found_idx = -1;
        for (int i = 0; i < leaf->num_records; i++) {
            char current_full_name[NAME_SIZE * 2];
            sprintf(current_full_name, "%s %s", leaf->records[i].name, leaf->records[i].lastname);
            if (leaf->records[i].birth_year == birth_year_key && strcmp(current_full_name, full_name) == 0) {
                found_idx = i;
                break;
            }
        }
        if (found_idx != -1) {
            for (int i = found_idx; i < leaf->num_records - 1; i++) leaf->records[i] = leaf->records[i + 1];
            leaf->num_records--;
            write_leaf_node(current_offset, leaf);
        }
        free(leaf);
        return;
    }

    InternalNode* node = read_internal_node(index_file, current_offset);
    if (!node) return;
    int i = 0;
    while (i < node->num_keys && birth_year_key >= node->keys[i]) i++;

    delete_recursive(index_file, node->children_pointers[i], birth_year_key, full_name);

    BPTHeader* header = read_header(index_file);
    int min_keys = (current_offset == header->root_offset) ? 1 : BTREE_ORDER - 1;
    if (get_node_key_count(index_file, node->children_pointers[i]) < min_keys) {
        handle_underflow(index_file, node->children_pointers[i], current_offset, i);
    }
    free(header);
    free(node);
}

static void handle_underflow(FILE* index_file, int node_offset, int parent_offset, int child_index_in_parent) {
    InternalNode* parent = read_internal_node(index_file, parent_offset);
    if (!parent) return;
    int min_keys = BTREE_ORDER - 1;

    if (child_index_in_parent > 0) {
        int prev_sibling_offset = parent->children_pointers[child_index_in_parent - 1];
        if (get_node_key_count(index_file, prev_sibling_offset) > min_keys) {
            case3a_redistribute(index_file, node_offset, prev_sibling_offset, parent_offset, child_index_in_parent, 1);
            free(parent);
            return;
        }
    }
    if (child_index_in_parent < parent->num_keys) {
        int next_sibling_offset = parent->children_pointers[child_index_in_parent + 1];
        if (get_node_key_count(index_file, next_sibling_offset) > min_keys) {
            case3a_redistribute(index_file, node_offset, next_sibling_offset, parent_offset, child_index_in_parent, 0);
            free(parent);
            return;
        }
    }

    if (child_index_in_parent > 0) {
        case3b_merge(index_file, node_offset, parent->children_pointers[child_index_in_parent - 1], parent_offset, child_index_in_parent);
    } else {
        case3b_merge(index_file, node_offset, parent->children_pointers[child_index_in_parent + 1], parent_offset, child_index_in_parent);
    }
    free(parent);
}

static void case3a_redistribute(FILE* index_file, int node_offset, int sibling_offset, int parent_offset, int child_index, int is_prev_sibling) {
    InternalNode* parent = read_internal_node(index_file, parent_offset);
    if (!parent) return;

    if (node_offset < 0) {
        LeafNode* node = read_leaf_node(node_offset);
        LeafNode* sibling = read_leaf_node(sibling_offset);
        if (!node || !sibling) { if(parent) free(parent); if(node) free(node); if(sibling) free(sibling); return; }

        if (is_prev_sibling) {
            for (int i = node->num_records; i > 0; i--) node->records[i] = node->records[i-1];
            node->records[0] = sibling->records[sibling->num_records - 1];
            node->num_records++;
            sibling->num_records--;
            parent->keys[child_index - 1] = node->records[0].birth_year;
        } else {
            node->records[node->num_records] = sibling->records[0];
            node->num_records++;
            for (int i = 0; i < sibling->num_records - 1; i++) sibling->records[i] = sibling->records[i+1];
            sibling->num_records--;
            parent->keys[child_index] = sibling->records[0].birth_year;
        }
        write_leaf_node(node_offset, node);
        write_leaf_node(sibling_offset, sibling);
        free(node);
        free(sibling);
    } else {
        InternalNode* node = read_internal_node(index_file, node_offset);
        InternalNode* sibling = read_internal_node(index_file, sibling_offset);
        if (!node || !sibling) { if(parent) free(parent); if(node) free(node); if(sibling) free(sibling); return; }

        if (is_prev_sibling) {
            for (int i = node->num_keys; i > 0; i--) node->keys[i] = node->keys[i-1];
            for (int i = node->num_keys + 1; i > 0; i--) node->children_pointers[i] = node->children_pointers[i-1];

            node->keys[0] = parent->keys[child_index - 1];
            node->children_pointers[0] = sibling->children_pointers[sibling->num_keys];
            node->num_keys++;

            parent->keys[child_index - 1] = sibling->keys[sibling->num_keys - 1];
            sibling->num_keys--;
        } else {
            node->keys[node->num_keys] = parent->keys[child_index];
            node->children_pointers[node->num_keys + 1] = sibling->children_pointers[0];
            node->num_keys++;

            parent->keys[child_index] = sibling->keys[0];

            for(int i = 0; i < sibling->num_keys - 1; i++) sibling->keys[i] = sibling->keys[i+1];
            for(int i = 0; i < sibling->num_keys; i++) sibling->children_pointers[i] = sibling->children_pointers[i+1];
            sibling->num_keys--;
        }
        write_internal_node(index_file, node_offset, node);
        write_internal_node(index_file, sibling_offset, sibling);
        free(node);
        free(sibling);
    }
    write_internal_node(index_file, parent_offset, parent);
    free(parent);
}

static void case3b_merge(FILE* index_file, int node_offset, int sibling_offset, int parent_offset, int child_index) {
    if(node_offset > sibling_offset && node_offset > 0){
        int temp = node_offset;
        node_offset = sibling_offset;
        sibling_offset = temp;
        child_index--;
    }

    InternalNode* parent = read_internal_node(index_file, parent_offset);
    if (!parent) return;

    int key_from_parent_idx = child_index;

    if (node_offset < 0) {
        LeafNode* node = read_leaf_node(node_offset);
        LeafNode* sibling = read_leaf_node(sibling_offset);
        if (!node || !sibling) { if(parent) free(parent); if(node) free(node); if(sibling) free(sibling); return; }

        for (int i = 0; i < sibling->num_records; i++) node->records[node->num_records++] = sibling->records[i];
        node->next_leaf_id = sibling->next_leaf_id;

        write_leaf_node(node_offset, node);
        free(sibling);
    } else {
        InternalNode* node = read_internal_node(index_file, node_offset);
        InternalNode* sibling = read_internal_node(index_file, sibling_offset);
        if (!node || !sibling) { if(parent) free(parent); if(node) free(node); if(sibling) free(sibling); return; }

        node->keys[node->num_keys] = parent->keys[key_from_parent_idx];
        node->num_keys++;

        for (int i = 0; i < sibling->num_keys; i++) {
            node->keys[node->num_keys] = sibling->keys[i];
            node->children_pointers[node->num_keys] = sibling->children_pointers[i];
            node->num_keys++;
        }
        node->children_pointers[node->num_keys] = sibling->children_pointers[sibling->num_keys];

        write_internal_node(index_file, node_offset, node);
        free(sibling);
    }

    for (int i = key_from_parent_idx; i < parent->num_keys - 1; i++) parent->keys[i] = parent->keys[i+1];
    for (int i = key_from_parent_idx + 1; i < parent->num_keys; i++) parent->children_pointers[i] = parent->children_pointers[i+1];
    parent->num_keys--;

    write_internal_node(index_file, parent_offset, parent);
    free(parent);
}

static int get_node_key_count(FILE* index_file, int offset) {
    if (offset < 0) {
        LeafNode* leaf = read_leaf_node(offset);
        int count = leaf ? leaf->num_records : 0;
        if (leaf) free(leaf);
        return count;
    }
    InternalNode* node = read_internal_node(index_file, offset);
    int count = node ? node->num_keys : 0;
    if (node) free(node);
    return count;
}

static int find_parent_offset(FILE *index_file, int root_offset, int child_offset) {
    if (root_offset == -1 || root_offset == child_offset) return -1;
    BPTHeader *header = read_header(index_file);
    if (!header) return -1;
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
            if (current_node->children_pointers[i] == child_offset) {
                parent_found_offset = current_offset;
                break;
            }
            if(current_node->children_pointers[i] >= 0){
                 if (tail < queue_size) queue[tail++] = current_node->children_pointers[i];
            }
        }
        free(current_node);
    }
    free(queue);
    free(header);
    return parent_found_offset;
}
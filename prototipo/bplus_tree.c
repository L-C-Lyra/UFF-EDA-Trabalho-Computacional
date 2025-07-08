#include "bplus_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Protótipos de Funções Estáticas ---
static int find_parent_offset(FILE *index_file, int current_offset, int child_offset);
static int get_node_key_count(FILE* index_file, int offset);
static char* get_node_key(FILE* index_file, int offset, int key_index, char* key_out);
static void get_leaf_key(FILE* file, int leaf_id, int record_index, char* key_out);

static PlayerLocation insert_into_parent(FILE *index_file, int parent_offset, const char* key, int left_child_pointer, int right_child_pointer);
static PlayerLocation insert_into_leaf_and_handle_split(FILE *index_file, int leaf_id, PlayerData record);

static void delete_recursive(FILE *index_file, int parent_offset, int current_offset, const char* full_name_key);
static void handle_underflow(FILE *index_file, int node_offset, int parent_offset);
static void redistribute_nodes(FILE* index_file, int parent_offset, int node_offset, int sibling_offset, int node_index, int sibling_index);
static void merge_nodes(FILE* index_file, int parent_offset, int node_offset, int sibling_offset, int node_index, int sibling_index);


// --- Função Auxiliar ---
static void get_full_name(const PlayerData* p, char* full_name_out) {
    if (p && (strlen(p->name) > 0 || strlen(p->lastname) > 0)) {
        sprintf(full_name_out, "%s %s", p->name, p->lastname);
    } else {
        full_name_out[0] = '\0';
    }
}

// --- Funções Principais da API ---

PlayerLocation bpt_insert(FILE *index_file, PlayerData record) {
    BPTHeader *header = read_header(index_file);
    PlayerLocation loc = {-1, -1};
    if(!header) return loc;

    if (header->root_offset == -1) {
        LeafNode *leaf = (LeafNode *)calloc(1, sizeof(LeafNode));
        leaf->num_records = 1;
        leaf->records[0] = record;
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

    char record_full_name[FULL_NAME_SIZE];
    get_full_name(&record, record_full_name);

    while (current_offset >= 0) {
        InternalNode *current_node = read_internal_node(index_file, current_offset);
        if(!current_node) return loc;
        int i = 0;
        while (i < current_node->num_keys && strcmp(record_full_name, current_node->keys[i]) >= 0) {
            i++;
        }
        int next_offset = current_node->children_pointers[i];
        free(current_node);

        if (next_offset < 0) {
            return insert_into_leaf_and_handle_split(index_file, next_offset, record);
        }
        current_offset = next_offset;
    }
    return loc;
}

PlayerData *bpt_search(FILE *index_file, const char* full_name_key) {
    BPTHeader *header = read_header(index_file);
    if (!header || header->root_offset == -1) {
        if(header) free(header);
        return NULL;
    }
    int current_ptr = header->root_offset;
    free(header);

    while (current_ptr >= 0) {
        InternalNode *current_node = read_internal_node(index_file, current_ptr);
        if (!current_node) return NULL;
        int i = 0;
        while (i < current_node->num_keys && strcmp(full_name_key, current_node->keys[i]) >= 0) {
            i++;
        }
        current_ptr = current_node->children_pointers[i];
        free(current_node);
    }

    LeafNode *leaf = read_leaf_node(current_ptr);
    if (!leaf) return NULL;

    PlayerData *record = NULL;
    for (int i = 0; i < leaf->num_records; i++) {
        char current_full_name[FULL_NAME_SIZE];
        get_full_name(&leaf->records[i], current_full_name);
        if (strcmp(full_name_key, current_full_name) == 0) {
            record = (PlayerData *)malloc(sizeof(PlayerData));
            if (record) memcpy(record, &leaf->records[i], sizeof(PlayerData));
            break;
        }
    }
    free(leaf);
    return record;
}

void bpt_delete(FILE* index_file, const char* full_name_key) {
    BPTHeader* header = read_header(index_file);
    if (!header || header->root_offset == -1) {
        if (header) free(header);
        return;
    }

    delete_recursive(index_file, -1, header->root_offset, full_name_key);

    InternalNode* root = read_internal_node(index_file, header->root_offset);
    if (root && root->num_keys == 0 && header->internal_node_count > 1) {
        header->root_offset = root->children_pointers[0];
        header->internal_node_count--; // A remoção do nó antigo deve ser tratada
        write_header(index_file, header);
    } else if (header->leaf_count == 1) {
        LeafNode* leaf = read_leaf_node(root->children_pointers[0]);
        if (leaf && leaf->num_records == 0) {
            // A árvore está completamente vazia
             init_BPT(index_file, BTREE_ORDER);
        }
        if(leaf) free(leaf);
    }


    if(root) free(root);
    free(header);
}

// --- Funções de Inserção e Split ---

static PlayerLocation insert_into_leaf_and_handle_split(FILE *index_file, int leaf_id, PlayerData record) {
    PlayerLocation loc = {-1, -1};
    LeafNode *leaf = read_leaf_node(leaf_id);
    if(!leaf) return loc;

    char record_full_name[FULL_NAME_SIZE];
    get_full_name(&record, record_full_name);

    if (leaf->num_records < KEY_SIZE) {
        int i = leaf->num_records - 1;
        while (i >= 0) {
            char current_record_full_name[FULL_NAME_SIZE];
            get_full_name(&leaf->records[i], current_record_full_name);
            if (strcmp(record_full_name, current_record_full_name) < 0) {
                leaf->records[i + 1] = leaf->records[i];
                i--;
            } else { break; }
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
    header->next_leaf_id--;
    header->leaf_count++;

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
    while (pos < KEY_SIZE) {
        char current_name[FULL_NAME_SIZE];
        get_full_name(&leaf->records[pos], current_name);
        if(strcmp(record_full_name, current_name) > 0) pos++;
        else break;
    }
    memcpy(temp_records, leaf->records, pos * sizeof(PlayerData));
    temp_records[pos] = record;
    memcpy(&temp_records[pos + 1], &leaf->records[pos], (KEY_SIZE - pos) * sizeof(PlayerData));

    int split_point = (KEY_SIZE + 1) / 2;
    leaf->num_records = split_point;
    new_leaf->num_records = (KEY_SIZE + 1) - split_point;

    memcpy(leaf->records, temp_records, split_point * sizeof(PlayerData));
    memset(&leaf->records[split_point], 0, (KEY_SIZE - split_point) * sizeof(PlayerData));
    memcpy(new_leaf->records, &temp_records[split_point], new_leaf->num_records * sizeof(PlayerData));

    if (pos < split_point) {
        loc.leaf_id = leaf_id;
        loc.record_index = pos;
    } else {
        loc.leaf_id = new_leaf_id;
        loc.record_index = pos - split_point;
    }

    write_leaf_node(leaf_id, leaf);
    write_leaf_node(new_leaf_id, new_leaf);

    char promoted_key[FULL_NAME_SIZE];
    get_full_name(&new_leaf->records[0], promoted_key);
    int parent_offset = find_parent_offset(index_file, header->root_offset, leaf_id);
    write_header(index_file, header);

    insert_into_parent(index_file, parent_offset, promoted_key, leaf_id, new_leaf_id);
    free(header);
    free(leaf);
    free(new_leaf);
    return loc;
}

static PlayerLocation insert_into_parent(FILE *index_file, int parent_offset, const char* key, int left_child_pointer, int right_child_pointer) {
    PlayerLocation loc = {-1, -1};
    BPTHeader *header = read_header(index_file);
    if (parent_offset == -1) {
        InternalNode *new_root = (InternalNode*)calloc(1, sizeof(InternalNode));
        int new_root_offset = header->next_free_offset;
        new_root->num_keys = 1;
        strcpy(new_root->keys[0], key);
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
        while (i >= 0 && strcmp(key, parent->keys[i]) < 0) {
            strcpy(parent->keys[i + 1], parent->keys[i]);
            parent->children_pointers[i + 2] = parent->children_pointers[i + 1];
            i--;
        }
        strcpy(parent->keys[i + 1], key);
        parent->children_pointers[i + 2] = right_child_pointer;
        parent->num_keys++;
        write_internal_node(index_file, parent_offset, parent);
        free(parent);
        free(header);
        return loc;
    }

    char temp_keys[KEY_SIZE + 1][FULL_NAME_SIZE];
    int temp_pointers[KEY_SIZE + 2];
    int pos = 0;
    while (pos < parent->num_keys && strcmp(key, parent->keys[pos]) > 0) pos++;
    
    memcpy(temp_keys, parent->keys, pos * sizeof(parent->keys[0]));
    memcpy(temp_pointers, parent->children_pointers, (pos + 1) * sizeof(int));
    
    strcpy(temp_keys[pos], key);
    temp_pointers[pos + 1] = right_child_pointer;
    
    memcpy(&temp_keys[pos + 1], &parent->keys[pos], (KEY_SIZE - pos) * sizeof(parent->keys[0]));
    memcpy(&temp_pointers[pos + 2], &parent->children_pointers[pos + 1], (parent->num_keys - pos) * sizeof(int));

    int split_point = KEY_SIZE / 2;
    char promoted_key[FULL_NAME_SIZE];
    strcpy(promoted_key, temp_keys[split_point]);

    InternalNode *new_internal = (InternalNode*)calloc(1, sizeof(InternalNode));
    int new_node_offset = header->next_free_offset;
    header->next_free_offset += sizeof(InternalNode);
    header->internal_node_count++;
    parent->num_keys = split_point;
    memcpy(parent->keys, temp_keys, split_point * sizeof(parent->keys[0]));
    memcpy(parent->children_pointers, temp_pointers, (split_point + 1) * sizeof(int));

    new_internal->num_keys = KEY_SIZE - split_point;
    memcpy(new_internal->keys, &temp_keys[split_point + 1], new_internal->num_keys * sizeof(new_internal->keys[0]));
    memcpy(new_internal->children_pointers, &temp_pointers[split_point + 1], (new_internal->num_keys + 1) * sizeof(int));

    write_internal_node(index_file, parent_offset, parent);
    write_internal_node(index_file, new_node_offset, new_internal);
    
    int grand_parent_offset = find_parent_offset(index_file, header->root_offset, parent_offset);
    
    write_header(index_file, header);

    insert_into_parent(index_file, grand_parent_offset, promoted_key, parent_offset, new_node_offset);
    free(header);
    free(parent);
    free(new_internal);
    return loc;
}

// --- Funções de Deleção e Balanceamento ---

static void delete_recursive(FILE *index_file, int parent_offset, int current_offset, const char* full_name_key) {
    int min_keys = (BTREE_ORDER - 1) / 2;
    if (current_offset >= 0) { // Nó Interno
        InternalNode* node = read_internal_node(index_file, current_offset);
        if (!node) return;
        int i = 0;
        while (i < node->num_keys && strcmp(full_name_key, node->keys[i]) >= 0) i++;

        delete_recursive(index_file, current_offset, node->children_pointers[i], full_name_key);

        // Checa underflow no filho após a recursão
        int child_key_count = get_node_key_count(index_file, node->children_pointers[i]);
        BPTHeader* header = read_header(index_file);
        int is_root = header->root_offset == current_offset;
        free(header);
        // Raiz pode ter menos chaves, os outros não.
        if (child_key_count < min_keys && !is_root) {
             handle_underflow(index_file, node->children_pointers[i], current_offset);
        }

        free(node);
    } else { // Nó Folha
        LeafNode* leaf = read_leaf_node(current_offset);
        if (!leaf) return;
        int found_idx = -1;
        for (int i = 0; i < leaf->num_records; i++) {
            char current_full_name[FULL_NAME_SIZE];
            get_full_name(&leaf->records[i], current_full_name);
            if (strcmp(current_full_name, full_name_key) == 0) {
                found_idx = i;
                break;
            }
        }
        if (found_idx != -1) {
            for (int i = found_idx; i < leaf->num_records - 1; i++) {
                leaf->records[i] = leaf->records[i + 1];
            }
            leaf->num_records--;
            write_leaf_node(current_offset, leaf);
        }
        free(leaf);
    }
}


static void handle_underflow(FILE* index_file, int node_offset, int parent_offset) {
    InternalNode* parent = read_internal_node(index_file, parent_offset);
    if (!parent) return;

    int node_index = -1, i;
    for (i = 0; i <= parent->num_keys; i++) {
        if (parent->children_pointers[i] == node_offset) {
            node_index = i;
            break;
        }
    }
    if (node_index == -1) { free(parent); return; }

    int min_keys = (BTREE_ORDER - 1) / 2;
    int sibling_offset;

    // Tenta redistribuir com irmão esquerdo
    if (node_index > 0) {
        sibling_offset = parent->children_pointers[node_index - 1];
        if (get_node_key_count(index_file, sibling_offset) > min_keys) {
            redistribute_nodes(index_file, parent_offset, node_offset, sibling_offset, node_index, node_index - 1);
            free(parent);
            return;
        }
    }

    // Tenta redistribuir com irmão direito
    if (node_index < parent->num_keys) {
        sibling_offset = parent->children_pointers[node_index + 1];
        if (get_node_key_count(index_file, sibling_offset) > min_keys) {
            redistribute_nodes(index_file, parent_offset, node_offset, sibling_offset, node_index, node_index + 1);
            free(parent);
            return;
        }
    }

    // Se não pode redistribuir, faz merge
    if (node_index > 0) {
        sibling_offset = parent->children_pointers[node_index - 1];
        merge_nodes(index_file, parent_offset, node_offset, sibling_offset, node_index, node_index - 1);
    } else {
        sibling_offset = parent->children_pointers[node_index + 1];
        merge_nodes(index_file, parent_offset, node_offset, sibling_offset, node_index, node_index + 1);
    }

    free(parent);
}

static void redistribute_nodes(FILE* index_file, int parent_offset, int node_offset, int sibling_offset, int node_index, int sibling_index) {
    InternalNode* parent = read_internal_node(index_file, parent_offset);
    if (!parent) return;

    if (node_offset < 0) { // É folha
        LeafNode* node = read_leaf_node(node_offset);
        LeafNode* sibling = read_leaf_node(sibling_offset);

        if (sibling_index < node_index) { // Irmão esquerdo
            for (int i = node->num_records; i > 0; i--) node->records[i] = node->records[i - 1];
            node->records[0] = sibling->records[sibling->num_records - 1];
            node->num_records++;
            sibling->num_records--;
            char new_parent_key[FULL_NAME_SIZE];
            get_full_name(&node->records[0], new_parent_key);
            strcpy(parent->keys[sibling_index], new_parent_key);
        } else { // Irmão direito
            node->records[node->num_records] = sibling->records[0];
            node->num_records++;
            for (int i = 0; i < sibling->num_records - 1; i++) sibling->records[i] = sibling->records[i + 1];
            sibling->num_records--;
            char new_parent_key[FULL_NAME_SIZE];
            get_full_name(&sibling->records[0], new_parent_key);
            strcpy(parent->keys[node_index], new_parent_key);
        }
        write_leaf_node(node_offset, node);
        write_leaf_node(sibling_offset, sibling);
        free(node);
        free(sibling);
    } else { // É nó interno
         InternalNode* node = read_internal_node(index_file, node_offset);
         InternalNode* sibling = read_internal_node(index_file, sibling_offset);
        if(sibling_index < node_index){ //Irmão esquerdo
            for(int i = node->num_keys; i > 0; i--) strcpy(node->keys[i], node->keys[i-1]);
            for(int i = node->num_keys + 1; i > 0; i--) node->children_pointers[i] = node->children_pointers[i-1];

            strcpy(node->keys[0], parent->keys[sibling_index]);
            node->children_pointers[0] = sibling->children_pointers[sibling->num_keys];
            node->num_keys++;

            strcpy(parent->keys[sibling_index], sibling->keys[sibling->num_keys-1]);
            sibling->num_keys--;
        } else { //Irmão direito
            strcpy(node->keys[node->num_keys], parent->keys[node_index]);
            node->children_pointers[node->num_keys + 1] = sibling->children_pointers[0];
            node->num_keys++;

            strcpy(parent->keys[node_index], sibling->keys[0]);
            for(int i = 0; i < sibling->num_keys - 1; i++) strcpy(sibling->keys[i], sibling->keys[i+1]);
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

static void merge_nodes(FILE* index_file, int parent_offset, int node_offset, int sibling_offset, int node_index, int sibling_index) {
    // Garante que node é o da direita e sibling o da esquerda
    if(sibling_index > node_index){
        int temp_off = node_offset;
        node_offset = sibling_offset;
        sibling_offset = temp_off;
        int temp_idx = node_index;
        node_index = sibling_index;
        sibling_index = temp_idx;
    }

    InternalNode* parent = read_internal_node(index_file, parent_offset);
    if (!parent) return;

    if (node_offset < 0) { // Merge de folhas
        LeafNode* node = read_leaf_node(node_offset);
        LeafNode* sibling = read_leaf_node(sibling_offset);
        for (int i = 0; i < node->num_records; i++) {
            sibling->records[sibling->num_records++] = node->records[i];
        }
        sibling->next_leaf_id = node->next_leaf_id;
        if (node->next_leaf_id != 0) {
            LeafNode* next_leaf = read_leaf_node(node->next_leaf_id);
            if(next_leaf) {
                next_leaf->prev_lead_id = sibling_offset;
                write_leaf_node(node->next_leaf_id, next_leaf);
                free(next_leaf);
            }
        }
        write_leaf_node(sibling_offset, sibling);
        free(node);
        free(sibling);
        // "Deletar" a folha vazia (aqui apenas conceitual)
    } else { // Merge de nós internos
        InternalNode* node = read_internal_node(index_file, node_offset);
        InternalNode* sibling = read_internal_node(index_file, sibling_offset);
        strcpy(sibling->keys[sibling->num_keys], parent->keys[sibling_index]);
        sibling->num_keys++;
        for(int i = 0; i < node->num_keys; i++){
            strcpy(sibling->keys[sibling->num_keys], node->keys[i]);
            sibling->children_pointers[sibling->num_keys] = node->children_pointers[i];
            sibling->num_keys++;
        }
        sibling->children_pointers[sibling->num_keys] = node->children_pointers[node->num_keys];
        write_internal_node(index_file, sibling_offset, sibling);
        free(node);
        free(sibling);
        // "Deletar" o nó interno vazio (conceitual)
    }

    // Atualiza o pai
    for(int i = sibling_index; i < parent->num_keys - 1; i++) strcpy(parent->keys[i], parent->keys[i+1]);
    for(int i = node_index; i < parent->num_keys; i++) parent->children_pointers[i] = parent->children_pointers[i+1];
    parent->num_keys--;
    write_internal_node(index_file, parent_offset, parent);
    free(parent);
}

// --- Funções Auxiliares de Navegação e Contagem ---

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


static int find_parent_offset(FILE *index_file, int current_offset, int child_offset) {
    if (current_offset < 0) return -1;
    InternalNode* node = read_internal_node(index_file, current_offset);
    if (!node) return -1;

    for (int i = 0; i <= node->num_keys; i++) {
        if (node->children_pointers[i] == child_offset) {
            free(node);
            return current_offset;
        }
    }

    int parent_found = -1;
    for (int i = 0; i <= node->num_keys; i++) {
        if (node->children_pointers[i] >= 0) {
             parent_found = find_parent_offset(index_file, node->children_pointers[i], child_offset);
             if (parent_found != -1) break;
        }
    }
    free(node);
    return parent_found;
}
#include "bplus_tree_io.h"

int find_parent_offset(FILE *index_file, int root_offset, int child_pointer) {
    int current_offset = root_offset;
    int parent_offset = -1;

    while(current_offset >= 0) {
        InternalNode *current_node = read_internal_node(index_file, current_offset);
        if(!current_node) break;

        // Verifica se algum dos filhos deste nó é o que estamos a procurar:
        for (int i = 0; i <= current_node->num_keys; i++) {
            if (current_node->children_pointers[i] == child_pointer) {
                parent_offset = current_offset;
                free(current_node);
                return parent_offset;
            }
        }

        // Se não encontrou, desce para o próximo nível:
        int i = 0;
        char temp_key[NAME_SIZE] = ""; // Chave temporária para descida.
        if(child_pointer < 0) { // Se o filho é uma folha, precisamos de um registro para guiar:
            LeafNode* leaf = read_leaf_node(child_pointer);
            if(leaf) {
                strcpy(temp_key, leaf->records[0].lastname);
                free(leaf);
            }
        } else { // Se o filho é um nó interno:
            InternalNode* child_node = read_internal_node(index_file, child_pointer);
            if(child_node){
                strcpy(temp_key, child_node->keys[0]);
                free(child_node);
            }
        }

        while(i < current_node->num_keys && strcmp(temp_key, current_node->keys[i]) >= 0) {
            i++;
        }
        parent_offset = current_offset; // Guarda o pai atual.
        current_offset = current_node->children_pointers[i];
        free(current_node);
    }
    return parent_offset; // Retorna o último pai visitado.
}

void insert_into_parent(FILE *index_file, int parent_offset, char *key, int right_child_pointer) {
    // Se não há pai (divisão da folha raiz original), cria uma raiz.

    InternalNode *parent = read_internal_node(index_file, parent_offset);
    if(!parent) return;

    // Caso 1: O nó pai tem espaço.
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
        return;
    }

    // Caso 2: Nó pai está cheio, precisa dividir.
    BPTHeader *header = read_header(index_file);

    // Arrays temporários para a divisão:
    char temp_keys[KEY_SIZE + 1][NAME_SIZE];
    int temp_pointers[KEY_SIZE + 2];
    int pos = 0;
    while (pos < KEY_SIZE && strcmp(key, parent->keys[pos]) > 0) pos++;

    // Copia os dados para os arrays temporários, inserindo a nova chave/ponteiro.
    memcpy(temp_keys, parent->keys, pos * NAME_SIZE);
    memcpy(temp_pointers, parent->children_pointers, (pos + 1) * sizeof(int));
    strcpy(temp_keys[pos], key);
    temp_pointers[pos + 1] = right_child_pointer;
    memcpy(&temp_keys[pos + 1], &parent->keys[pos], (KEY_SIZE - pos) * NAME_SIZE);
    memcpy(&temp_pointers[pos + 2], &parent->children_pointers[pos + 1], (KEY_SIZE - pos + 1) * sizeof(int));

    int split_point = KEY_SIZE / 2;
    char promoted_key[NAME_SIZE];
    strcpy(promoted_key, temp_keys[split_point]);

    // Cria um nó interno no arquivo de índice:
    InternalNode *new_internal = (InternalNode*)calloc(1, sizeof(InternalNode));
    int new_node_offset = header->next_free_offset;

    // Atualiza o nó pai (antigo):
    parent->num_keys = split_point;
    memcpy(parent->keys, temp_keys, split_point * NAME_SIZE);
    memcpy(parent->children_pointers, temp_pointers, (split_point + 1) * sizeof(int));

    // Preenche o novo nó interno:
    new_internal->num_keys = KEY_SIZE - split_point;
    memcpy(new_internal->keys, &temp_keys[split_point + 1], new_internal->num_keys * NAME_SIZE);
    memcpy(new_internal->children_pointers, &temp_pointers[split_point + 1], (new_internal->num_keys + 1) * sizeof(int));

    write_internal_node(index_file, parent_offset, parent);
    write_internal_node(index_file, new_node_offset, new_internal);

    // Caso especial: A divisão foi na raiz.
    if (parent_offset == header->root_offset) {
        InternalNode *new_root = (InternalNode*)calloc(1, sizeof(InternalNode));
        int new_root_offset = header->next_free_offset + sizeof(InternalNode);

        new_root->num_keys = 1;
        strcpy(new_root->keys[0], promoted_key);
        new_root->children_pointers[0] = parent_offset;
        new_root->children_pointers[1] = new_node_offset;

        write_internal_node(index_file, new_root_offset, new_root);

        // Atualiza o header para a nova raiz e novos contadores/offsets:
        header->root_offset = new_root_offset;
        header->internal_node_count += 2; // O pai dividido + o novo
        header->next_free_offset += (2*sizeof(InternalNode));
        free(new_root);
    } else {
        // Chamada recursiva para inserir a chave promovida no avô:
        int grand_parent_offset = find_parent_offset(index_file, header->root_offset, parent_offset);
        insert_into_parent(index_file, grand_parent_offset, promoted_key, new_node_offset);
        header->internal_node_count++;
        header->next_free_offset += sizeof(InternalNode);
    }

    write_header(index_file, header);

    free(header);
    free(parent);
    free(new_internal);
}

void insert_into_leaf_and_handle_split(FILE *index_file, int leaf_id, PlayerData record) {
    LeafNode *leaf = read_leaf_node(leaf_id);
    if(!leaf) return;

    // Caso 1: A folha tem espaço.
    if (leaf->num_records < KEY_SIZE) {
        int i = leaf->num_records - 1;
        while (i >= 0 && strcmp(record.lastname, leaf->records[i].lastname) < 0) {
            memcpy(&leaf->records[i + 1], &leaf->records[i], sizeof(PlayerData));
            i--;
        }
        memcpy(&leaf->records[i + 1], &record, sizeof(PlayerData));
        leaf->num_records++;
        write_leaf_node(leaf_id, leaf); // Escreve a folha modificada de volta para o seu arquivo.
        free(leaf);
        return;
    }

    // Caso 2: A folha está cheia, precisa dividir.
    BPTHeader *header = read_header(index_file);

    // Cria a nova folha e aloca um novo ID para ela:
    LeafNode *new_leaf = (LeafNode *)calloc(1, sizeof(LeafNode));
    int new_leaf_id = header->next_leaf_id;

    // Atualiza o encadeamento entre as folhas:
    new_leaf->next_leaf_id = leaf->next_leaf_id;
    new_leaf->prev_lead_id = leaf_id;
    leaf->next_leaf_id = new_leaf_id;
    if(new_leaf->next_leaf_id != -1) {
        LeafNode *next_leaf = read_leaf_node(new_leaf->next_leaf_id);
        if(next_leaf){
            next_leaf->prev_lead_id = new_leaf_id;
            write_leaf_node(new_leaf->next_leaf_id, next_leaf);
            free(next_leaf);
        }
    }

    // Usa um array temporário para ordenar todos os registros (antigos + o novo):
    PlayerData temp_records[KEY_SIZE + 1];
    int pos = 0;
    while (pos < KEY_SIZE && strcmp(record.lastname, leaf->records[pos].lastname) > 0) pos++;
    memcpy(temp_records, leaf->records, pos * sizeof(PlayerData));
    memcpy(&temp_records[pos], &record, sizeof(PlayerData));
    memcpy(&temp_records[pos + 1], &leaf->records[pos], (KEY_SIZE - pos) * sizeof(PlayerData));

    // Divide os registros entre a folha antiga e a nova:
    int split_point = (KEY_SIZE + 1) / 2;
    leaf->num_records = split_point;
    new_leaf->num_records = (KEY_SIZE + 1) - split_point;

    memcpy(leaf->records, temp_records, split_point * sizeof(PlayerData));
    memcpy(new_leaf->records, &temp_records[split_point], new_leaf->num_records * sizeof(PlayerData));

    // Escreve as duas folhas nos seus respectivos arquivos:
    write_leaf_node(leaf_id, leaf);
    write_leaf_node(new_leaf_id, new_leaf);

    // A chave a ser promovida é o nome do primeiro jogador na nova folha:
    char promoted_key[NAME_SIZE];
    strcpy(promoted_key, new_leaf->records[0].lastname);

    // Encontra o nó pai para inserir a chave promovida:
    int parent_offset = find_parent_offset(index_file, header->root_offset, leaf_id);
    insert_into_parent(index_file, parent_offset, promoted_key, new_leaf_id);

    // Atualiza e escreve o header:
    header->leaf_count++;
    header->next_leaf_id--;
    write_header(index_file, header);

    free(header);
    free(leaf);
    free(new_leaf);
}

void bpt_insert(FILE *index_file, PlayerData record) {
    BPTHeader *header = read_header(index_file);

    // Caso 1: A árvore está completamente vazia.
    if (header->root_offset == -1) {
        // Cria a primeira folha no próprio arquivo.
        LeafNode *leaf = (LeafNode *)calloc(1, sizeof(LeafNode));
        leaf->num_records = 1;
        leaf->next_leaf_id = -1;
        leaf->prev_lead_id = -1;
        memcpy(&leaf->records[0], &record, sizeof(PlayerData));

        int new_leaf_id = -1; // A primeira folha tem ID -1.
        write_leaf_node(new_leaf_id, leaf);
        free(leaf);

        // Cria o primeiro nó interno (raiz) no arquivo de índice.
        InternalNode *root = (InternalNode *)calloc(1, sizeof(InternalNode));
        root->num_keys = 0; // A raiz só aponta para a folha.
        root->children_pointers[0] = new_leaf_id;

        // Atualiza o header com os novos metadados:
        header->root_offset = sizeof(BPTHeader);
        header->internal_node_count = 1;
        header->leaf_count = 1;
        header->next_free_offset = sizeof(BPTHeader) + sizeof(InternalNode);
        header->next_leaf_id = -2; // Próximo ID de folha disponível.

        write_internal_node(index_file, header->root_offset, root);
        write_header(index_file, header);

        free(root);
        free(header);
        return;
    }

    // Caso 2: A árvore já existe. Encontrar a folha correta navegando em disco.
    int current_offset = header->root_offset;
    free(header); // Libera o header, vamos reler conforme necessário.

    // Navega pelos nós internos até encontrar um ponteiro para uma folha (ponteiro negativo).
    while (current_offset >= 0) {
        InternalNode *current_node = read_internal_node(index_file, current_offset);
        if(!current_node) return; // Segurança.

        int i = 0;
        // Compara a chave de inserção com as chaves do nó interno para decidir qual caminho seguir.
        while (i < current_node->num_keys && strcmp(record.lastname, current_node->keys[i]) >= 0) {
            i++;
        }
        current_offset = current_node->children_pointers[i];
        free(current_node); // Libera o nó lido da memória.
    }

    // current_offset agora contém o ID da folha (um número negativo).
    int leaf_id_to_insert = current_offset;

    // Chama a função que insere na folha e trata a divisão se necessário.
    insert_into_leaf_and_handle_split(index_file, leaf_id_to_insert, record);
}

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
        if (strcmp(key, leaf->records[i].lastname) == 0) {
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
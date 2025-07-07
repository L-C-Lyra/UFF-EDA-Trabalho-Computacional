#include "hash_table.h" 
#include <string.h>     
#include <stdio.h>      
#include <stdlib.h>     


HashTable* create_hash_table() {
    HashTable *ht = (HashTable*) malloc(sizeof(HashTable));
    if (ht == NULL) 
        return NULL;
    
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        ht->list_heads[i] = NULL;
    }
    return ht;
}

unsigned int hash_string(const char *str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // deslocando 5 bits pra esquerda, 2ˆ5
    }
    return hash % HASH_TABLE_SIZE; // hash dentro do tamanho da tabela
}


int hash_table_insert(HashTable *ht, const char *full_name, int leaf_id, int record_index_in_leaf) {
    if (ht == NULL || full_name == NULL) 
        return 0; // 
    

    unsigned int index = hash_string(full_name);

    // para verificar duplicatas (precisa?)
    HashEntry *current = ht->list_heads[index];
    while (current != NULL) {
        if (strcmp(current->full_name, full_name) == 0) 
            return 0;
        
        current = current->next;
    }

   
    HashEntry *new_entry = (HashEntry*) malloc(sizeof(HashEntry));
    if (new_entry == NULL) {
        return 0;
    }
    strncpy(new_entry->full_name, full_name, sizeof(new_entry->full_name) - 1);
    new_entry->full_name[sizeof(new_entry->full_name) - 1] = '\0'; 
    new_entry->leaf_id = leaf_id;
    new_entry->record_index_in_leaf = record_index_in_leaf;
    new_entry->next = NULL;

    new_entry->next = ht->list_heads[index];
    ht->list_heads[index] = new_entry;

    return 1; // Inserido com sucesso
}

// search nova entrada 
int hash_table_search(HashTable *ht, const char *full_name, int *leaf_id, int *record_index_in_leaf) {
    if (ht == NULL || full_name == NULL) {
        return 0; 
    }

    unsigned int index = hash_string(full_name);
    HashEntry *current = ht->list_heads[index]; 

    while (current != NULL) {
        if (strcmp(current->full_name, full_name) == 0) {
            if (leaf_id != NULL) *leaf_id = current->leaf_id;
            if (record_index_in_leaf != NULL) *record_index_in_leaf = current->record_index_in_leaf;
            return 1; // achou
        }
        current = current->next;
    }

    return 0; 
}

void hash_table_delete(HashTable* ht, const char* full_name) {
    if (!ht || !full_name) return;

    unsigned int index = hash_string(full_name);

    HashEntry* current = ht->list_heads[index];
    HashEntry* prev = NULL;

    while (current != NULL) {
        if (strcmp(current->full_name, full_name) == 0) {
            if (prev == NULL) {
                ht->list_heads[index] = current->next;
            }
            else {
                prev->next = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

void free_hash_table(HashTable *ht) {
    if (ht == NULL) {
        return;
    }
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashEntry *current = ht->list_heads[i]; 
        while (current != NULL) {
            HashEntry *temp = current;
            current = current->next;
            free(temp);
        }
        ht->list_heads[i] = NULL; 
    }
    free(ht); 
}
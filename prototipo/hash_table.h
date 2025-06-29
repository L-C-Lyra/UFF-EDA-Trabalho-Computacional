#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include "bplus_tree_io.h" 


#define HASH_TABLE_SIZE 1009 // um número primo 

// struct para um nó da lista encadeada (para tratamento de colisões)
typedef struct HashEntry {
    char full_name[NAME_SIZE * 2]; 
    int leaf_id;                   // id da folha na B+ 
    int record_index_in_leaf;      // indice do registro dentro da folha
    struct HashEntry *next;        // ponteiro para o próximo nó na lista encadeada
} HashEntry;


typedef struct HashTable {
    HashEntry *list_heads[HASH_TABLE_SIZE]; // array de ponteiros para os cabeçalhos das linked list
} HashTable;


HashTable* create_hash_table();

unsigned int hash_string(const char *str);


int hash_table_insert(HashTable *ht, const char *full_name, int leaf_id, int record_index_in_leaf);

int hash_table_search(HashTable *ht, const char *full_name, int *leaf_id, int *record_index_in_leaf);

void free_hash_table(HashTable *ht);

#endif 

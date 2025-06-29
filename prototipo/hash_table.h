#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include "bplus_tree_io.h" 
#define HASH_TABLE_SIZE 1009 


// cada nó tem a chave (player full name) e o "valor" (localização na B+ Tree)
typedef struct HashEntry {
    char full_name[NAME_SIZE * 2]; 
    int leaf_id;                  // corresponde a um arqv binario distinto
    int record_index_in_leaf;      // indice do registro dentro da folha pra achar o jogador especifico
    struct HashEntry *next;        // ponteiro para o próximo nó na lde
} HashEntry;


typedef struct HashTable {

    HashEntry *buckets[HASH_TABLE_SIZE]; // array de ponteiros (lde)

} HashTable;



HashTable* create_hash_table();


unsigned int hash_string(const char *str); // uma funcao pra string


int hash_table_insert(HashTable *ht, const char *full_name, int leaf_id, int record_index_in_leaf);

int hash_table_search(HashTable *ht, const char *full_name, int *leaf_id, int *record_index_in_leaf);

void free_hash_table(HashTable *ht);

#endif 
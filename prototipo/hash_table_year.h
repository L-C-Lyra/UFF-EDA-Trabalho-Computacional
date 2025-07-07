#ifndef HASH_TABLE_YEAR_H
#define HASH_TABLE_YEAR_H

#define HASH_TABLE_SIZE 1009

#include "bplus_tree_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct PlayerLocationNode {
    char nationality[NAME_SIZE];
    int leaf_id;
    int record_index_in_leaf;
    struct PlayerLocationNode *next;
} PlayerLocationNode;

typedef struct HashEntryYear {
    int birth_year;
    PlayerLocationNode *player_list_head;
    struct HashEntryYear *next;
} HashEntryYear;

typedef struct HashTableYear {
    HashEntryYear *list_heads[HASH_TABLE_SIZE];
} HashTableYear;

HashTableYear *create_hash_table_year();
PlayerLocationNode *search_hash_table_year(HashTableYear *ht, int year);
void insert_hash_table_year(HashTableYear *ht, int birth_year, char *nationality, int leaf_id, int record_index);
void free_hash_table_year(HashTableYear *ht);

#endif
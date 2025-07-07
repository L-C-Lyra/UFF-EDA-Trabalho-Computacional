#include "hash_table_year.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

HashTableYear *create_hash_table_year() {
    HashTableYear *ht = (HashTableYear *)malloc(sizeof(HashTableYear));
    if (ht == NULL) return NULL;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        ht->list_heads[i] = NULL;
    }
    return ht;
}

unsigned int hash_function_year(int year) {
    return (unsigned int) year % HASH_TABLE_SIZE;
}

HashEntryYear *search_hash_table_year(HashTableYear *ht, int year) {
    unsigned int index = hash_function_year(year);
    HashEntryYear *year_entry = ht->list_heads[index];

    while (year_entry != NULL) {
        if (year_entry->birth_year == year) {
            return year_entry;
        }
        year_entry = year_entry->next;
    }
    return NULL;
}

void insert_hash_table_year(HashTableYear *ht, int birth_year, char *nationality, int leaf_id, int record_index) {
    unsigned int index = hash_function_year(birth_year); 
    HashEntryYear *year_entry = ht->list_heads[index];
    while (year_entry != NULL && year_entry->birth_year != birth_year) {
        year_entry = year_entry->next;
    }

    if (year_entry == NULL) {
        year_entry = (HashEntryYear *)malloc(sizeof(HashEntryYear));
        
        year_entry->birth_year = birth_year;
        year_entry->player_list_head = NULL;

        year_entry->next = ht->list_heads[index];
        ht->list_heads[index] = year_entry;
    }

    PlayerLocationNode *new_player = (PlayerLocationNode *)malloc(sizeof(PlayerLocationNode));

    strncpy(new_player->nationality, nationality, NAME_SIZE);
    new_player->nationality[NAME_SIZE - 1] = '\0';
    new_player->leaf_id = leaf_id;
    new_player->record_index_in_leaf = record_index;

    new_player->next = year_entry->player_list_head;
    year_entry->player_list_head = new_player;
}

void free_hash_table_year(HashTableYear *ht) {
    if (ht == NULL) return;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashEntryYear *current_year_entry = ht->list_heads[i];
        while (current_year_entry != NULL) {
            PlayerLocationNode *current_player_node = current_year_entry->player_list_head;
            while (current_player_node != NULL) {
                PlayerLocationNode *temp_player = current_player_node;
                current_player_node = current_player_node->next;
                free(temp_player);
            }
            HashEntryYear *temp_year = current_year_entry;
            current_year_entry = current_year_entry->next;
            free(temp_year);
        }
    }
    free(ht);
}
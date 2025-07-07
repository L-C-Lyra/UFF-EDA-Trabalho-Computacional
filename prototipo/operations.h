#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "bplus_tree.h"
#include "hash_table.h"
#include "tabela_hash_pais.h"
#include "hash_table_year.h"

void search_player_by_name(HashTable* player_ht, const char* full_name);
void search_players_by_country(TabelaHashPais* country_ht, const char* country_name);
void search_players_by_year(HashTableYear *year_ht, int year);

void delete_player_by_name(FILE* index_file, HashTable* player_ht, TabelaHashPais* country_ht, const char* full_name);
void delete_players_by_country(FILE* index_file, HashTable* player_ht, TabelaHashPais* country_ht, const char* country_name);

#endif
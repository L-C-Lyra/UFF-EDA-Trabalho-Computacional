#ifndef BPLUS_TREE_IO
#define BPLUS_TREE_IO

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define NAME_SIZE 32
#define FILENAME_SIZE 16
#define BTREE_ORDER 3
#define KEY_SIZE (2*BTREE_ORDER - 1)

/*
------------------------
        STRUCTS
------------------------
*/

typedef struct PlayerData {
    char name[NAME_SIZE]; 
    char lastname[NAME_SIZE]; 
    int birth_year; 
    int death_year;
    char nacionality[NAME_SIZE];
    int best_rank;
    int best_rank_year;
    int points;
    int is_retired;
} PlayerData;
// PlayerData size = 120 bytes

typedef struct InternalNode {
    int num_keys;
    int keys[KEY_SIZE]; //Editado para utilizar birth_year como chave
    int children_pointers[KEY_SIZE+1];
} InternalNode;
// InternalNode size = 48 bytes

typedef struct LeafNode {
    int num_records;
    int prev_lead_id;
    int next_leaf_id;
    PlayerData records[KEY_SIZE];
} LeafNode;
// LeafNode size = 612 bytes

typedef struct BPTHeader {
    int order;
    int root_offset;
    int internal_node_count;
    int leaf_count;
    int next_free_offset;
    int next_leaf_id;
} BPTHeader;
// BPTHeader size = 24 bytes

/*
-----------------------------------------------------
        FUNÇÕES DE ESCRITA/LEITURA DE NÓS
-----------------------------------------------------
*/

int init_BPT(FILE *index_file, int order);
void write_header(FILE *index_file, BPTHeader *header);
BPTHeader *read_header(FILE *index_file);
void write_internal_node(FILE *index_file, int node_offset, InternalNode *node);
InternalNode *read_internal_node(FILE *index_file, int node_offset);
void write_leaf_node(int leaf_id, LeafNode *node);
LeafNode *read_leaf_node(int leaf_id);

/*
-----------------------------------------------------
        FUNÇÕES QUE NÃO FARÃO PARTE DA API
-----------------------------------------------------
*/
void write_internal_node_from_id(FILE *index_file, int node_id, InternalNode *node);
InternalNode *read_internal_node_from_id(FILE *index_file, int node_id);
void add_internal_node(FILE *index_file, InternalNode *node);
void add_leaf_node(FILE *index_file, LeafNode *leafnode);
void print_header(BPTHeader *header);
void print_internal_nodes(FILE *index_file);
void print_leafs(BPTHeader *header);
void print_leaf_aux(LeafNode *node);
void print_player(PlayerData *record);
void print_players_by_status(FILE *index_file, int is_retired_status);
#endif
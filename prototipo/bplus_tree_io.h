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
} PlayerData;
// PlayerData size = 116 bytes

typedef struct InternalNode {
    char is_pointer_to_leaf;
    int num_keys;
    char keys[KEY_SIZE][NAME_SIZE];
    int children_pointers[KEY_SIZE+1];
} InternalNode;
// InternalNode size = 192 bytes

typedef struct LeafNode {
    int num_records;
    int prev_lead_id;
    int next_leaf_id;
    PlayerData records[KEY_SIZE];
} LeafNode;
// LeafNode size = 592 bytes

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

int init_BPT(FILE *index_file);
void write_header(FILE *index_file, BPTHeader *header);
void read_header(FILE *index_file, BPTHeader *header);
void write_internal_node(FILE *index_file, int offset, InternalNode *node);
void read_internal_node(FILE *index_file, int offset, InternalNode *node);
void write_leaf_node(int leaf_id, LeafNode *node);
void read_leaf_node(int leaf_id, LeafNode *node);

/*
-----------------------------------------------------
        FUNÇÕES QUE NÃO FARÃO PARTE DA API
-----------------------------------------------------
*/
// void add_internal_node(FILE *index_file, InternalNode *node);
// void add_leaf_node(FILE *index_file, LeafNode *leafnode);
// void print_header(BPTHeader *header);
// void print_internal_nodes(FILE *index_file);
// void print_leafs(BPTHeader *header);
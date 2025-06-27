#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define NAME_SIZE 32
#define FILENAME_SIZE 16
#define BTREE_ORDER 3
#define KEY_SIZE (2*BTREE_ORDER - 1)

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
// InternalNode size = 196 bytes

typedef struct LeafNode {
    int num_records;
    int prev_lead_id;
    int next_leaf_id;
    PlayerData records[KEY_SIZE];
} LeafNode;
// LeafNode size = 596 bytes

typedef struct BPTHeader {
    int order;
    int root_node_offset;
    int internal_node_count;
    int leaf_file_count;
    int next_free_offset;
    int next_leaf_id;
} BPTHeader;
// BPTHeader size = 24 bytes

// FUNÇÕES ESCRITA/LEITURA

void write_header(FILE *index_file, BPTHeader *header);
void read_header(FILE *index_file, BPTHeader *header);
void init_header(FILE *index_file);
void print_header(BPTHeader *header);
void write_internal_node(FILE *index_file, int offset, InternalNode *node);
void read_internal_node(FILE *index_file, int offset, InternalNode *node);
void print_node_aux(InternalNode *node);
void print_internal_nodes(FILE *index_file);
void add_internal_node(FILE *index_file, InternalNode *node);
void write_leaf_node(int leaf_id, LeafNode *node);
void read_leaf_node(int leaf_id, LeafNode *node);
void print_leaf_aux(LeafNode *node);
int init_BPT(FILE *index_file);
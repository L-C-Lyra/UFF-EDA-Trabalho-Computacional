#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define NAME_SIZE 32
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
    char is_leaf;
    int num_keys;
    char keys[KEY_SIZE][NAME_SIZE];
    int children_pointers[KEY_SIZE+1];
    char is_pointer_to_leaf;
} InternalNode;
// InternalNode size = 196 bytes

typedef struct LeafNode {
    char is_leaf;
    int num_records;
    PlayerData records[KEY_SIZE];
    int prev_lead_id;
    int next_leaf_id;
} LeafNode;
// LeafNode size = 596 bytes

typedef struct BPTHeader {
    int order;
    int root_node_offset;
    int internal_node_count;
    int leaf_file_count;
    int next_internal_node_id;
    int next_leaf_id;
} BPTHeader;
// BPTHeader size = 24 bytes
#ifndef BPLUS_TREE_H
#define BPLUS_TREE_H
#include "bplus_tree_io.h"

void bpt_insert(FILE *index_file, PlayerData record);

PlayerData *bpt_search(FILE *index_file, char *key);

#endif
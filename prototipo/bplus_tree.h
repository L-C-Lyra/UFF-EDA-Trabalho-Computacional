#ifndef BPLUS_TREE_H
#define BPLUS_TREE_H
#include "bplus_tree_io.h"

// Insere um registro de jogador na árvore.
void bpt_insert(FILE *index_file, PlayerData record);

// Busca um jogador pelo ANO DE NASCIMENTO (a nova chave).
// Retorna o primeiro registro encontrado com aquele ano.
PlayerData *bpt_search(FILE *index_file, int birth_year_key);

#endif
#ifndef BPLUS_TREE_H
#define BPLUS_TREE_H

#include "bplus_tree_io.h"

// Struct para retornar a localização de um registo após a inserção.
typedef struct {
    int leaf_id;
    int record_index;
} PlayerLocation;

// A função de inserção agora retorna a localização do registo inserido.
PlayerLocation bpt_insert(FILE *index_file, PlayerData record);

// Busca um jogador pelo ANO DE NASCIMENTO.
PlayerData *bpt_search(FILE *index_file, int birth_year_key);

// Remove um jogador da árvore pela sua chave (ano) e nome completo.
void bpt_delete(FILE* index_file, int birth_year_key, const char* full_name);

#endif
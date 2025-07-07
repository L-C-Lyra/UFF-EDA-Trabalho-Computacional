#ifndef TABELA_HASH_PAIS_H
#define TABELA_HASH_PAIS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tabela_hash_pais.h"
#include "bplus_tree_io.h" 

#define COUNTRY_HASH_TABLE_SIZE 257

typedef struct NoLocalizacaoJogador {
    char nome_completo[NAME_SIZE * 2];
    int id_folha;
    int indice_registro_na_folha;
    struct NoLocalizacaoJogador *proximo;
} NoLocalizacaoJogador;

typedef struct EntradaHashPais {
    char nacionalidade[NAME_SIZE];
    NoLocalizacaoJogador *lista_jogadores_cabeca;
    struct EntradaHashPais *proximo;
} EntradaHashPais;

typedef struct TabelaHashPais {
    EntradaHashPais *list_heads[COUNTRY_HASH_TABLE_SIZE];
} TabelaHashPais;


TabelaHashPais* criar_tabela_hash_pais();

unsigned int hash_string_pais(const char *str);

int inserir_tabela_hash_pais(TabelaHashPais *cht, const char *nacionalidade, const char *nome_completo, int id_folha, int indice_registro_na_folha);

NoLocalizacaoJogador* buscar_tabela_hash_pais(TabelaHashPais *cht, const char *nacionalidade);

void deletar_tabela_hash_pais(TabelaHashPais* cht, const char* nacionalidade, const char* full_name);

void liberar_tabela_hash_pais(TabelaHashPais *cht);

#endif 
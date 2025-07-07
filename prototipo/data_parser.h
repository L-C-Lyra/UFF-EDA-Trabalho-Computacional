#ifndef DATA_PARSER_H
#define DATA_PARSER_H

#include "bplus_tree_io.h" 

void trim_whitespace(char *str);
PlayerData parse_player_data(char *line);

#endif
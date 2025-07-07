cmake_minimum_required(VERSION 3.31)
project(prototipo C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(prototipo
        bplus_tree.c
        bplus_tree.h
        bplus_tree_io.c
        bplus_tree_io.h
        data_parser.c
        data_parser.h
        hash_table.c
        hash_table.h
        main.c
        # main_hash_test.c
        tabela_hash_pais.c
        tabela_hash_pais.h
        title_manager.h
        title_manager.c
        operations.h
        operations.c
        # teste.c
        )

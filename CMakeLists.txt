cmake_minimum_required(VERSION 3.5)
project(UFF_EDA_Trabalho_Computacional C)

set(CMAKE_C_STANDARD 11)

include_directories(libs)
include_directories(src)

option(BUILD_TESTS "Build test executables" OFF)

#se adicionar novas libs inclua-as aqui
set(LIB_SOURCES
        libs/BinaryTree.c
        libs/BinarySearchTree.c
        libs/LinkedList.c
        libs/BPlusTree.c
        libs/HashUtils.c
        libs/HashMap.c
)
set(LIB_HEADERS
        libs/BinaryTree.h
        libs/BinarySearchTree.h
        libs/LinkedList.h
        libs/GenericTypes.h
        libs/GenericImplementations.h
        libs/BPlusTree.h
        libs/HashUtils.h
        libs/HashMap.h
)

#inclua src aqui
set(SRC_SOURCES
        src/Main.c
        src/models/ChampionshipInfo.c
        src/models/EstandarteAward.c
        src/models/SchoolInfo.c
        src/models/IndividualInfo.c
)
set(SRC_HEADERS
        src/models/ChampionshipInfo.h
        src/models/EstandarteAward.h
        src/models/SchoolInfo.h
        src/models/IndividualInfo.h
)

if(BUILD_TESTS)
    enable_testing()
    add_executable(testBinaryTree test/TestBinaryTree.c ${LIB_SOURCES}) 
    add_executable(testBinarySearchTree test/TestBinarySearchTree.c ${LIB_SOURCES})
    add_executable(testLinkedList test/TestLinkedList.c ${LIB_SOURCES}) 
    add_executable(testBPlusTree test/TestBPlusTree.c ${LIB_SOURCES})
    add_executable(testHashUtils test/TestHashUtils.c ${LIB_SOURCES})
    add_executable(testHashMap test/TestHashMap.c ${LIB_SOURCES})
    add_test(NAME BinaryTreeTest COMMAND TestBinaryTree)
    add_test(NAME BinarySearchTreeTest COMMAND TestBinarySearchTree)
    add_test(NAME LinkedListTest COMMAND TestLinkedList)
    add_test(NAME BPlusTreeTest COMMAND TestBPlusTree)
    add_test(NAME HashUtilsTest COMMAND TestHashUtils)
    add_test(NAME HashMapTest COMMAND TestHashMap)
    add_custom_target(runAllTests
            COMMAND ${CMAKE_CTEST_COMMAND} --verbose
            DEPENDS TestBinaryTree TestBinarySearchTree TestLinkedList testBPlusTree testHashUtils testHashMap
    )
    
else()
    add_executable(UFF_EDA_Trabalho_Computacional
            ${SRC_SOURCES}
            ${SRC_HEADERS}
            ${LIB_SOURCES}
            ${LIB_HEADERS}
    )
endif()

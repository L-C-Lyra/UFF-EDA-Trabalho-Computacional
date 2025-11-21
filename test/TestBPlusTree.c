#include <stdio.h>
#include <assert.h>
#include "../libs/BPlusTree.h"
#include "Tester.h"

//AVISO: os testes não foram gerados com IA mas IA foi usado para formatar os textos de uma forma mais coesa!

void testBPlusTreeInitialize() {
    BPlusTree* tree = bPlusTreeInitialize();
    runTest("bPlusTreeInitialize", tree == NULL);
}
void testBPlusTreeCreate() {
    int t = 3;
    BPlusTree* tree = bPlusTreeCreate(t);
    
    runTest("bPlusTreeCreate - not NULL", tree != NULL);
    runTest("bPlusTreeCreate - numKeys is 0", tree->numKeys == 0);
    runTest("bPlusTreeCreate - isLeaf is 1", tree->isLeaf == 1);
    runTest("bPlusTreeCreate - next is NULL", tree->next == NULL);
    
    bPlusTreeFree(tree);
}

void testBPlusTreeInsert() {
    int t = 3;
    BPlusTree* tree = bPlusTreeInitialize();
    
    tree = bPlusTreeInsert(tree, 10, t);
    runTest("bPlusTreeInsert - first element", tree != NULL && tree->numKeys == 1);
    
    tree = bPlusTreeInsert(tree, 20, t);
    runTest("bPlusTreeInsert - second element", tree->numKeys == 2);
    
    tree = bPlusTreeInsert(tree, 5, t);
    tree = bPlusTreeInsert(tree, 15, t);
    tree = bPlusTreeInsert(tree, 25, t);
    
    runTest("bPlusTreeInsert - multiple elements", tree->numKeys > 0);
    tree = bPlusTreeInsert(tree, 10, t);
    runTest("bPlusTreeInsert - duplicate element", tree != NULL);
    
    bPlusTreeFree(tree);
}

void testBPlusTreeSearch() {
    int t = 3;
    BPlusTree* tree = bPlusTreeInitialize();
    
    tree = bPlusTreeInsert(tree, 10, t);
    tree = bPlusTreeInsert(tree, 20, t);
    tree = bPlusTreeInsert(tree, 5, t);
    tree = bPlusTreeInsert(tree, 15, t);
    
    BPlusTree* found = bPlusTreeSearch(tree, 10);
    runTest("bPlusTreeSearch - existing element", found != NULL);
    
    found = bPlusTreeSearch(tree, 99);
    runTest("bPlusTreeSearch - non-existing element", found == NULL);
    
    found = bPlusTreeSearch(NULL, 10);
    runTest("bPlusTreeSearch - empty tree", found == NULL);
    
    bPlusTreeFree(tree);
}

void testBPlusTreeRemove() {
    int t = 3;
    BPlusTree* tree = bPlusTreeInitialize();
    
    tree = bPlusTreeInsert(tree, 10, t);
    tree = bPlusTreeInsert(tree, 20, t);
    tree = bPlusTreeInsert(tree, 5, t);
    tree = bPlusTreeInsert(tree, 15, t);
    tree = bPlusTreeInsert(tree, 25, t);
    tree = bPlusTreeInsert(tree, 30, t);
    
    tree = bPlusTreeRemove(tree, 15, t);
    BPlusTree* found = bPlusTreeSearch(tree, 15);
    runTest("bPlusTreeRemove - existing element", found == NULL);
    
    tree = bPlusTreeRemove(tree, 99, t);
    runTest("bPlusTreeRemove - non-existing element", tree != NULL);
    
    bPlusTreeFree(tree);
}

void testBPlusTreePrint() {
    int t = 3;
    BPlusTree* tree = bPlusTreeInitialize();
    
    tree = bPlusTreeInsert(tree, 10, t);
    tree = bPlusTreeInsert(tree, 20, t);
    tree = bPlusTreeInsert(tree, 5, t);
    tree = bPlusTreeInsert(tree, 15, t);
    tree = bPlusTreeInsert(tree, 25, t);
    
    printf("\nTesting print functions (visual verification required):\n");
    printf("Tree structure:\n");
    bPlusTreePrint(tree);
    printf("\n");
    
    printf("Keys in order: ");
    bPlusTreePrintKeys(tree);
    
    runTest("bPlusTreePrint functions", 1);
    
    bPlusTreeFree(tree);
}

void testBPlusTreeStress() {
    int t = 4;
    BPlusTree* tree = bPlusTreeInitialize();
    
    int values[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    int numValues = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < numValues; i++) {
        tree = bPlusTreeInsert(tree, values[i], t);
    }
    
    int allFound = 1;
    for (int i = 0; i < numValues; i++) {
        if (bPlusTreeSearch(tree, values[i]) == NULL) {
            allFound = 0;
            break;
        }
    }
    runTest("bPlusTreeStress - all inserted elements found", allFound);
    
    for (int i = 0; i < numValues / 2; i++) {
        tree = bPlusTreeRemove(tree, values[i], t);
    }
    
    int removedNotFound = 1;
    for (int i = 0; i < numValues / 2; i++) {
        if (bPlusTreeSearch(tree, values[i]) != NULL) {
            removedNotFound = 0;
            break;
        }
    }
    runTest("bPlusTreeStress - removed elements not found", removedNotFound);
    
    bPlusTreeFree(tree);
}

int main() {
    printf("Running BPlusTree Tests...\n");
    printf("============================\n");
    testBPlusTreeInitialize();
    testBPlusTreeCreate();
    testBPlusTreeInsert();
    testBPlusTreeSearch();
    testBPlusTreeRemove();
    testBPlusTreePrint();
    testBPlusTreeStress();
    
    printResults();
    return (testsPassed == testsTotal) ? 0 : 1;
}
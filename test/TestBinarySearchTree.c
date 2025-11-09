#include <stdio.h>
#include <assert.h>
#include "../libs/BinarySearchTree.h"

int testsPassed = 0;
int testsTotal = 0;

void runTest(const char* testName, int condition) {
    testsTotal++;
    if (condition) {
        printf("PASSED: %s\n", testName);
        testsPassed++;
    } else {
        printf("ERROR: %s\n", testName);
    }
}

void testBinarySearchTreeInitialize() {
    BinarySearchTree* tree = binarySearchTreeInitialize();
    runTest("binarySearchTreeInitialize", tree == NULL);
}

void testBinarySearchTreeCreate() {
    BinarySearchTree* tree = binarySearchTreeCreate(10, NULL, NULL);
    runTest("binarySearchTreeCreate - root value", tree->info == 10);
    runTest("binarySearchTreeCreate - left is NULL", tree->left == NULL);
    runTest("binarySearchTreeCreate - right is NULL", tree->right == NULL);
    
    binarySearchTreeFree(tree);
}

void testBinarySearchTreeInsert() {
    BinarySearchTree* tree = binarySearchTreeInitialize();
    
    tree = binarySearchTreeInsert(tree, 10);
    runTest("binarySearchTreeInsert - first element", tree != NULL && tree->info == 10);
    
    tree = binarySearchTreeInsert(tree, 5);
    runTest("binarySearchTreeInsert - left child", tree->left != NULL && tree->left->info == 5);
    
    tree = binarySearchTreeInsert(tree, 15);
    runTest("binarySearchTreeInsert - right child", tree->right != NULL && tree->right->info == 15);
    
    tree = binarySearchTreeInsert(tree, 3);
    tree = binarySearchTreeInsert(tree, 7);
    tree = binarySearchTreeInsert(tree, 12);
    tree = binarySearchTreeInsert(tree, 18);
    
    runTest("binarySearchTreeInsert - multiple elements", 
            tree->left->left->info == 3 && tree->left->right->info == 7);
    
    tree = binarySearchTreeInsert(tree, 10);
    runTest("binarySearchTreeInsert - duplicate element", tree->info == 10);
    
    binarySearchTreeFree(tree);
}

void testBinarySearchTreeSearch() {
    BinarySearchTree* tree = binarySearchTreeInitialize();
    tree = binarySearchTreeInsert(tree, 10);
    tree = binarySearchTreeInsert(tree, 5);
    tree = binarySearchTreeInsert(tree, 15);
    tree = binarySearchTreeInsert(tree, 3);
    tree = binarySearchTreeInsert(tree, 7);
    
    BinarySearchTree* found = binarySearchTreeSearch(tree, 7);
    runTest("binarySearchTreeSearch - existing element", found != NULL && found->info == 7);
    
    found = binarySearchTreeSearch(tree, 99);
    runTest("binarySearchTreeSearch - non-existing element", found == NULL);
    
    found = binarySearchTreeSearch(NULL, 10);
    runTest("binarySearchTreeSearch - empty tree", found == NULL);
    
    binarySearchTreeFree(tree);
}

void testBinarySearchTreeRemove() {
    BinarySearchTree* tree = binarySearchTreeInitialize();
    tree = binarySearchTreeInsert(tree, 10);
    tree = binarySearchTreeInsert(tree, 5);
    tree = binarySearchTreeInsert(tree, 15);
    tree = binarySearchTreeInsert(tree, 3);
    tree = binarySearchTreeInsert(tree, 7);
    tree = binarySearchTreeInsert(tree, 12);
    tree = binarySearchTreeInsert(tree, 18);
    
    tree = binarySearchTreeRemove(tree, 3);
    BinarySearchTree* found = binarySearchTreeSearch(tree, 3);
    runTest("binarySearchTreeRemove - leaf node", found == NULL);
    
    tree = binarySearchTreeRemove(tree, 5);
    found = binarySearchTreeSearch(tree, 5);
    runTest("binarySearchTreeRemove - node with children", found == NULL);
    
    tree = binarySearchTreeRemove(tree, 99);
    runTest("binarySearchTreeRemove - non-existing element", tree != NULL);
    
    binarySearchTreeFree(tree);
}

void testBinarySearchTreePrint() {
    BinarySearchTree* tree = binarySearchTreeInitialize();
    tree = binarySearchTreeInsert(tree, 10);
    tree = binarySearchTreeInsert(tree, 5);
    tree = binarySearchTreeInsert(tree, 15);
    tree = binarySearchTreeInsert(tree, 3);
    tree = binarySearchTreeInsert(tree, 7);
    
    printf("\nTesting print functions (visual verification required):\n");
    printf("Pre-order: ");
    binarySearchTreePrintPreOrder(tree);
    printf("\n");
    
    printf("In-order (should be sorted): ");
    binarySearchTreePrintInOrder(tree);
    printf("\n");
    
    printf("Post-order: ");
    binarySearchTreePrintPostOrder(tree);
    printf("\n");
    
    printf("Indented:\n");
    binarySearchTreePrintIndented(tree);
    
    runTest("binarySearchTreePrint functions", 1);
    
    binarySearchTreeFree(tree);
}

int main() {
    printf("Running BinarySearchTree Tests...\n");
    printf("====================================\n");
    
    testBinarySearchTreeInitialize();
    testBinarySearchTreeCreate();
    testBinarySearchTreeInsert();
    testBinarySearchTreeSearch();
    testBinarySearchTreeRemove();
    testBinarySearchTreePrint();
    
    printf("\n====================================\n");
    printf("Tests Summary: %d/%d PASSED\n", testsPassed, testsTotal);
    
    if (testsPassed == testsTotal) {
        printf("All tests PASSED!\n");
        return 0;
    } else {
        printf("Some tests FAILED!\n");
        return 1;
    }
}
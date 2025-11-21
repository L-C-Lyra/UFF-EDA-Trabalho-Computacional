#include <stdio.h>
#include <assert.h>
#include "../libs/BinaryTree.h"
#include "Tester.h"

//AVISO: os testes não foram gerados com IA mas IA foi usado para formatar os textos de uma forma mais coesa!

void testBinaryTreeInitialize() {
    BinaryTree* tree = binaryTreeInitialize();
    runTest("binaryTreeInitialize", tree == NULL);
}

void testBinaryTreeCreate() {
    BinaryTree* left = binaryTreeCreate(2, NULL, NULL);
    BinaryTree* right = binaryTreeCreate(3, NULL, NULL);
    BinaryTree* root = binaryTreeCreate(1, left, right);
    
    runTest("binaryTreeCreate - root value", root->info == 1);
    runTest("binaryTreeCreate - left child", root->left->info == 2);
    runTest("binaryTreeCreate - right child", root->right->info == 3);
    
    binaryTreeFree(root);
}

void testBinaryTreeSearch() {
    BinaryTree* left = binaryTreeCreate(2, NULL, NULL);
    BinaryTree* right = binaryTreeCreate(3, NULL, NULL);
    BinaryTree* root = binaryTreeCreate(1, left, right);
    
    BinaryTree* found = binaryTreeSearch(root, 2);
    runTest("binaryTreeSearch - existing element", found != NULL && found->info == 2);
    
    found = binaryTreeSearch(root, 99);
    runTest("binaryTreeSearch - non-existing element", found == NULL);
    
    found = binaryTreeSearch(NULL, 1);
    runTest("binaryTreeSearch - empty tree", found == NULL);
    
    binaryTreeFree(root);
}

void testBinaryTreeHeight() {
    BinaryTree* tree = binaryTreeInitialize();
    runTest("binaryTreeHeight - empty tree", binaryTreeHeight(tree) == -1);
    
    tree = binaryTreeCreate(1, NULL, NULL);
    runTest("binaryTreeHeight - single node", binaryTreeHeight(tree) == 0);
    
    BinaryTree* left = binaryTreeCreate(2, NULL, NULL);
    BinaryTree* right = binaryTreeCreate(3, NULL, NULL);
    binaryTreeFree(tree);
    tree = binaryTreeCreate(1, left, right);
    runTest("binaryTreeHeight - height 1", binaryTreeHeight(tree) == 1);
    
    BinaryTree* leftLeft = binaryTreeCreate(4, NULL, NULL);
    tree->left->left = leftLeft;
    runTest("binaryTreeHeight - height 2", binaryTreeHeight(tree) == 2);
    
    binaryTreeFree(tree);
}

void testBinaryTreePrint() {
    BinaryTree* left = binaryTreeCreate(2, NULL, NULL);
    BinaryTree* right = binaryTreeCreate(3, NULL, NULL);
    BinaryTree* root = binaryTreeCreate(1, left, right);
    
    printf("\nTesting print functions (check visually for errors!):\n");
    printf("Pre-order: ");
    binaryTreePrintPreOrder(root);
    printf("\n");
    
    printf("In-order: ");
    binaryTreePrintInOrder(root);
    printf("\n");
    
    printf("Post-order: ");
    binaryTreePrintPostOrder(root);
    printf("\n");
    
    printf("Indented:\n");
    binaryTreePrintIndented(root);
    
    runTest("binaryTreePrint functions", 1);
    
    binaryTreeFree(root);
}

int main() {
    printf("Running BinaryTree Tests...\n");
    printf("================================\n");
    testBinaryTreeInitialize();
    testBinaryTreeCreate();
    testBinaryTreeSearch();
    testBinaryTreeHeight();
    testBinaryTreePrint();
    printResults();
    return (testsPassed == testsTotal) ? 0 : 1;
}
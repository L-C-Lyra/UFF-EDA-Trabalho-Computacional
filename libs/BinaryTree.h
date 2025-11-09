#include <stdio.h>
#include <stdlib.h>

typedef struct binaryNode{
  int info;
  struct binaryNode *left, *right;
}BinaryTree;

BinaryTree *binaryTreeInitialize(void);
BinaryTree *binaryTreeCreate(int root, BinaryTree *left, BinaryTree *right);

void binaryTreePrintPreOrder(BinaryTree *a);
void binaryTreePrintPostOrder(BinaryTree *a);
void binaryTreePrintInOrder(BinaryTree *a);
void binaryTreePrintIndented(BinaryTree *a);

void binaryTreeFree(BinaryTree *a);

BinaryTree *binaryTreeSearch(BinaryTree *a, int elem);

int binaryTreeHeight(BinaryTree *a);
#include <stdio.h>
#include <stdlib.h>

typedef struct binarySearchNode{
  int info;
  struct binarySearchNode *left, *right;
}BinarySearchTree;

BinarySearchTree *binarySearchTreeInitialize(void);
BinarySearchTree *binarySearchTreeCreate(int root, BinarySearchTree *left, BinarySearchTree *right);

void binarySearchTreePrintPreOrder(BinarySearchTree *a);
void binarySearchTreePrintPostOrder(BinarySearchTree *a);
void binarySearchTreePrintInOrder(BinarySearchTree *a);
void binarySearchTreePrintIndented(BinarySearchTree *a);

void binarySearchTreeFree(BinarySearchTree *a);

BinarySearchTree *binarySearchTreeSearch(BinarySearchTree *a, int elem);

BinarySearchTree *binarySearchTreeInsert(BinarySearchTree *a, int elem);
BinarySearchTree *binarySearchTreeRemove(BinarySearchTree *a, int info);
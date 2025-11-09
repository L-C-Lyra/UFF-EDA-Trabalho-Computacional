#include <stdio.h>
#include <stdlib.h>

typedef struct bPlusNode{
  int numKeys, isLeaf, *key;
  struct bPlusNode **child, *next;
}BPlusTree;

BPlusTree *bPlusTreeCreate(int t);
BPlusTree *bPlusTreeInitialize(void);
BPlusTree *bPlusTreeSearch(BPlusTree *a, int mat);
BPlusTree *bPlusTreeInsert(BPlusTree *T, int mat, int t);
BPlusTree* bPlusTreeRemove(BPlusTree* arv, int k, int t);
void bPlusTreeFree(BPlusTree *a);
void bPlusTreePrint(BPlusTree *a);
void bPlusTreePrintKeys(BPlusTree *a);
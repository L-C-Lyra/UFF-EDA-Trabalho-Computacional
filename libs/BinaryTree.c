#include "BinaryTree.h"

BinaryTree *binaryTreeInitialize(void){
  return NULL;
}

BinaryTree *binaryTreeCreate(int root, BinaryTree *left, BinaryTree *right){
  BinaryTree *new = (BinaryTree *) malloc(sizeof(BinaryTree));
  new->info = root;
  new->left = left;
  new->right = right;
  return new;
}

BinaryTree *binaryTreeSearch(BinaryTree *a, int elem){
  if((!a) || (a->info == elem)) return a;
  BinaryTree *resp = binaryTreeSearch(a->left, elem);
  if(resp) return resp;
  return binaryTreeSearch(a->right, elem);
}

void binaryTreePrintPreOrder(BinaryTree *a){
  if(a){
    printf("%d ", a->info);
    binaryTreePrintPreOrder(a->left);
    binaryTreePrintPreOrder(a->right);
  }
}

void binaryTreePrintPostOrder(BinaryTree *a){
  if(a){
    binaryTreePrintPostOrder(a->left);
    binaryTreePrintPostOrder(a->right);
    printf("%d ", a->info);
  }
}

void binaryTreePrintInOrder(BinaryTree *a){
  if(a){
    binaryTreePrintInOrder(a->left);
    printf("%d ", a->info);
    binaryTreePrintInOrder(a->right);
  }
}

void binaryTreePrintAux(BinaryTree *a, int level){
  int j;
  if(a){
    binaryTreePrintAux(a->right, level + 1);
    for(j = 0; j <= level; j++) printf("\t");
    printf("%d\n", a->info);
    binaryTreePrintAux(a->left, level + 1);
  }
  else{
   for(j = 0; j <= level; j++) printf("\t");
   printf("N\n");
  }
}

void binaryTreePrintIndented(BinaryTree *a){
  binaryTreePrintAux(a, 0);
}

void binaryTreeFree(BinaryTree *a){
  if(a){
    binaryTreeFree(a->left);
    binaryTreeFree(a->right);
    free(a);
  }
}

int max(int x, int y){
  if(x >= y) return x;
  return y;
}

int binaryTreeHeight(BinaryTree *a){
  if(!a) return -1;
  return max(binaryTreeHeight(a->left), binaryTreeHeight(a->right)) + 1;
}
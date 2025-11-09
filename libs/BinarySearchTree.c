#include "BinarySearchTree.h"

BinarySearchTree *binarySearchTreeInitialize(void){
  return NULL;
}

BinarySearchTree *binarySearchTreeCreate(int root, BinarySearchTree *left, BinarySearchTree *right){
  BinarySearchTree *new = (BinarySearchTree *) malloc(sizeof(BinarySearchTree));
  new->info = root;
  new->left = left;
  new->right = right;
  return new;
}

void binarySearchTreePrintPreOrder(BinarySearchTree *a){
  if(a){
    printf("%d ", a->info);
    binarySearchTreePrintPreOrder(a->left);
    binarySearchTreePrintPreOrder(a->right);
  }
}

void binarySearchTreePrintPostOrder(BinarySearchTree *a){
  if(a){
    binarySearchTreePrintPostOrder(a->left);
    binarySearchTreePrintPostOrder(a->right);
    printf("%d ", a->info);
  }
}

void binarySearchTreePrintInOrder(BinarySearchTree *a){
  if(a){
    binarySearchTreePrintInOrder(a->left);
    printf("%d ", a->info);
    binarySearchTreePrintInOrder(a->right);
  }
}

void binarySearchTreePrintAux(BinarySearchTree *a, int level){
  int j;
  if(a){
    binarySearchTreePrintAux(a->right, level + 1);
    for(j = 0; j <= level; j++) printf("\t");
    printf("%d\n", a->info);
    binarySearchTreePrintAux(a->left, level + 1);
  }
  else{
   for(j = 0; j <= level; j++) printf("\t");
   printf("N\n");
  }
}

void binarySearchTreePrintIndented(BinarySearchTree *a){
  binarySearchTreePrintAux(a, 0);
}

void binarySearchTreeFree(BinarySearchTree *a){
  if(a){
    binarySearchTreeFree(a->left);
    binarySearchTreeFree(a->right);
    free(a);
  }
}

BinarySearchTree *binarySearchTreeSearch(BinarySearchTree *a, int elem){
  if((!a) || (a->info == elem)) return a;
  if(a->info > elem) return binarySearchTreeSearch(a->left, elem);
  return binarySearchTreeSearch(a->right, elem);
}

BinarySearchTree *binarySearchTreeInsert(BinarySearchTree *a, int elem){
  if(!a) return binarySearchTreeCreate(elem, NULL, NULL);
  if(a->info > elem) a->left = binarySearchTreeInsert(a->left, elem);
  else if (a->info < elem) a->right = binarySearchTreeInsert(a->right, elem);
  return a;
}

BinarySearchTree *binarySearchTreeRemove(BinarySearchTree *a, int info){
  if(!a) return a;
  if(info < a->info) 
    a->left = binarySearchTreeRemove(a->left, info);
  else if(info > a->info) 
    a->right = binarySearchTreeRemove(a->right, info);
  else{
    if((!a->left) && (!a->right)){
      free(a);
      a = NULL;
    }
    else if((!a->left) || (!a->right)){
      BinarySearchTree *temp = a;
      if(!a->left) a = a->right;
      else a = a->left;
      free(temp); 
    }
    else{
      BinarySearchTree *child = a->left;
      while(child->right) child = child->right;
      a->info = child->info;
      child->info = info;
      a->left = binarySearchTreeRemove(a->left, info);
    }
  }
  return a; 
}
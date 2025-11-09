#include "BPlusTree.h"

BPlusTree *bPlusTreeCreate(int t){
  BPlusTree* new  = (BPlusTree*)malloc(sizeof(BPlusTree));
  new->numKeys = 0;
  new->key =(int*)malloc(sizeof(int)*((t*2)-1));
  new->isLeaf = 1;
  new->child = (BPlusTree**)malloc(sizeof(BPlusTree*)*t*2);
  new->next = NULL;
  int i;
  for(i=0; i<(t*2); i++) new->child[i] = NULL;
  return new;
}

BPlusTree *bPlusTreeInitialize(void){
  return NULL;
}

void bPlusTreeFree(BPlusTree *a){
  if(a){
    if(!a->isLeaf){
      int i;
      for(i = 0; i <= a->numKeys; i++) bPlusTreeFree(a->child[i]);
    }
    free(a->child);
    free(a->key);
    free(a);
  }
}

BPlusTree *bPlusTreeSearch(BPlusTree *a, int mat){
  if (!a) return NULL;
  int i = 0;
  while ((i < a->numKeys) && (mat > a->key[i])) i++;
  if ((i < a->numKeys) && (a->isLeaf) && (mat == a->key[i])) return a;
  if (a-> isLeaf) return NULL;
  if (a->key[i] == mat) i++;
  return bPlusTreeSearch(a->child[i], mat);
}

void bPlusTreePrintKeys(BPlusTree *a){
  if(!a) return;
  BPlusTree *p = a;
  while(p->child[0]) p = p->child[0];
  while(p){
    int i;
    for(i = 0; i < p->numKeys; i++) printf("%d ", p->key[i]);
    p = p->next;
  }
  printf("\n");
}

void bPlusTreePrintAux(BPlusTree *a, int level){
  if(a){
    int i,j;
    bPlusTreePrintAux(a->child[a->numKeys],level+1);
    for(i=a->numKeys-1; i >= 0; i--){
      for(j=0; j<=level; j++) printf("\t");
      printf("%d\n", a->key[i]);
      bPlusTreePrintAux(a->child[i],level+1);
    }
  }
}

void bPlusTreePrint(BPlusTree *a){
  bPlusTreePrintAux(a, 0);
}

void bPlusTreePrintAux0(BPlusTree *a, int level){
  if(a){
    int i,j;
    for(i=0; i<=a->numKeys-1; i++){
      bPlusTreePrintAux(a->child[i],level+1);
      for(j=0; j<=level; j++) printf("\t");
      printf("%d\n", a->key[i]);
    }
    bPlusTreePrintAux(a->child[i],level+1);
  }
}

void bPlusTreePrint0(BPlusTree *a){
  bPlusTreePrintAux0(a, 0);
}

BPlusTree *split(BPlusTree *x, int i, BPlusTree* y, int t){
  BPlusTree *z = bPlusTreeCreate(t);
  z->isLeaf = y->isLeaf;
  int j;
  if(!y->isLeaf){
    z->numKeys = t-1;
    for(j=0;j<t-1;j++) z->key[j] = y->key[j+t];
    for(j=0;j<t;j++){
      z->child[j] = y->child[j+t];
      y->child[j+t] = NULL;
    }
  }
  else {
    z->numKeys = t;
    for(j=0;j < t;j++) z->key[j] = y->key[j+t-1];
    z->next = y->next;
    y->next = z;
  }
  y->numKeys = t-1;
  for(j=x->numKeys; j>=i; j--) x->child[j+1]=x->child[j];
  x->child[i] = z;
  for(j=x->numKeys; j>=i; j--) x->key[j] = x->key[j-1];
  x->key[i-1] = y->key[t-1];
  x->numKeys++;
  return x;
}

BPlusTree *insertNonFull(BPlusTree *x, int mat, int t){
  int i = x->numKeys-1;
  if(x->isLeaf){
    while((i>=0) && (mat < x->key[i])){
      x->key[i+1] = x->key[i];
      i--;
    }
    x->key[i+1] = mat;
    x->numKeys++;
    return x;
  }
  while((i>=0) && (mat < x->key[i])) i--;
  i++;
  if(x->child[i]->numKeys == ((2*t)-1)){
    x = split(x, (i+1), x->child[i], t);
    if(mat > x->key[i]) i++;
  }
  x->child[i] = insertNonFull(x->child[i], mat, t);
  return x;
}

BPlusTree *bPlusTreeInsert(BPlusTree *T, int mat, int t){
  if(bPlusTreeSearch(T, mat)) return T;
  if(!T){
    T=bPlusTreeCreate(t);
    T->key[0] = mat;
    T->numKeys=1;
    return T;
  }
  if(T->numKeys == (2*t)-1){
    BPlusTree *S = bPlusTreeCreate(t);
    S->numKeys=0;
    S->isLeaf = 0;
    S->child[0] = T;
    S = split(S,1,T,t);
    S = insertNonFull(S, mat, t);
    return S;
  }
  T = insertNonFull(T, mat, t);
  return T;
}

BPlusTree* removeNode(BPlusTree* arv, int ch, int t){
  if(!arv) return arv;
  int i;
  for(i = 0; i < arv->numKeys && arv->key[i] < ch; i++);
  if((i < arv->numKeys) && (ch == arv->key[i]) && (arv->isLeaf)){
    printf("\nCASE 1\n");
    int j;
    for(j=i; j<arv->numKeys-1;j++) arv->key[j] = arv->key[j+1];
    arv->numKeys--;
    if(!arv->numKeys){
      bPlusTreeFree(arv);
      arv = NULL;
    }
    return arv;      
  }

  if((i < arv->numKeys) && (ch == arv->key[i])) i++;
  BPlusTree *y = arv->child[i], *z = NULL;
  if (y->numKeys == t-1){
    if((i < arv->numKeys) && (arv->child[i+1]->numKeys >=t)){
      printf("\nCASE 3A: i less than numKeys\n");
      z = arv->child[i+1];
      if(!y->isLeaf){
        y->key[t-1] = arv->key[i];
        arv->key[i] = z->key[0];
      }
      else{ 
        arv->key[i] = z->key[0] + 1;
        y->key[t-1] = z->key[0];
      }
      y->numKeys++;

      int j;
      for(j=0; j < z->numKeys-1; j++)
        z->key[j] = z->key[j+1];
      y->child[y->numKeys] = z->child[0];
      for(j=0; j < z->numKeys; j++)
        z->child[j] = z->child[j+1];
      z->numKeys--;
      arv->child[i] = removeNode(arv->child[i], ch, t);
      return arv;
    }
    if((i > 0) && (!z) && (arv->child[i-1]->numKeys >= t)){
      printf("\nCASE 3A: i equals numKeys\n");
      z = arv->child[i-1];
      int j;
      for(j = y->numKeys; j>0; j--)
        y->key[j] = y->key[j-1];
      for(j = y->numKeys+1; j>0; j--)
        y->child[j] = y->child[j-1];

      if(!y->isLeaf){
        y->key[0] = arv->key[i-1];
        arv->key[i-1] = z->key[z->numKeys - 1];
      }
      else{ 
        arv->key[i-1] = z->key[z->numKeys - 1];
        y->key[0] = z->key[z->numKeys-1];
      }
      y->numKeys++;

      y->child[0] = z->child[z->numKeys];
      z->numKeys--;
      arv->child[i] = removeNode(y, ch, t);
      return arv;
    }
    if(!z){
      if(i < arv->numKeys && arv->child[i+1]->numKeys == t-1){
        printf("\nCASE 3B: i less than numKeys\n");
        z = arv->child[i+1];
        if(!y->isLeaf){
          y->key[t-1] = arv->key[i];
          y->numKeys++;
        }
        int j = 0;
        while(j < t-1){
          if(!y->isLeaf) y->key[t+j] = z->key[j];
          else y->key[t+j-1] = z->key[j];
          y->numKeys++;
          j++;
        }
        y->next = z->next;
        if(!y->isLeaf){
          for(j=0; j<t; j++){
            y->child[t+j] = z->child[j];
            z->child[j] = NULL;
          }
        }
        bPlusTreeFree(z);
        for(j=i; j < arv->numKeys-1; j++){
          arv->key[j] = arv->key[j+1];
          arv->child[j+1] = arv->child[j+2];
        }
        arv->child[arv->numKeys] = NULL;
        arv->numKeys--;
        if(!arv->numKeys){
          BPlusTree *temp = arv;
          arv = arv->child[0];
          temp->child[0] = NULL;
          bPlusTreeFree(temp);
        }
        arv = removeNode(arv, ch, t);
        return arv;
      }
      if((i > 0) && (arv->child[i-1]->numKeys == t-1)){ 
        printf("\nCASE 3B: i equals numKeys\n");
        z = arv->child[i-1];
        if(!y->isLeaf){
          if(i == arv->numKeys){
            z->key[t-1] = arv->key[i-1];
          }else{
            z->key[t-1] = arv->key[i];
          }
          z->numKeys++;
        }
        int j = 0;
        while(j < t-1){
          if(!y->isLeaf) z->key[t+j] = y->key[j];
          else z->key[t+j-1] = y->key[j];
          z->numKeys++;
          j++;
        }
        z->next = y->next; 
        if(!z->isLeaf){
          for(j=0; j<t; j++){
            z->child[t+j] = y->child[j];
            y->child[j] = NULL;
          }
        }
        bPlusTreeFree(y);
        arv->child[arv->numKeys] = NULL;
        arv->numKeys--;
        if(!arv->numKeys){
          BPlusTree *temp = arv;
          arv = arv->child[0];
          temp->child[0] = NULL;
          bPlusTreeFree(temp);
          arv = removeNode(arv, ch, t);
        }
        else{
          i--;
          arv->child[i] = removeNode(arv->child[i], ch, t);
        }
        return arv;
      }
    }
  }  
  arv->child[i] = removeNode(arv->child[i], ch, t);
  return arv;
}

BPlusTree* bPlusTreeRemove(BPlusTree* arv, int k, int t){
  if(!arv || !bPlusTreeSearch(arv, k)) return arv;
  return removeNode(arv, k, t);
}
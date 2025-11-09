#include "LinkedList.h"

LinkedList* linkedListInitialize(void){
  return NULL;
}

LinkedList* linkedListInsert(LinkedList *l, int elem){
  LinkedList *new = (LinkedList *) malloc(sizeof(LinkedList));
  new->next = l;
  new->info = elem;
  return new;
}

void linkedListInsertVoid(LinkedList **l, int elem){
  LinkedList *new = (LinkedList *) malloc(sizeof(LinkedList));
  new->next = *l;
  new->info = elem;
  (*l) = new;
}

void linkedListPrint(LinkedList *l){
  LinkedList *p = l;
  while(p){
    printf("%d ", p->info);
    p = p->next;
  } 
}

void linkedListPrintRecursive(LinkedList *l){
  if(l){
    printf("%d ", l->info);
    linkedListPrintRecursive(l->next);
  }
}

void linkedListPrintRecursiveReverse(LinkedList *l){
  if(l){
    linkedListPrintRecursiveReverse(l->next);
    printf("%d ", l->info);
  }
}

void linkedListFree(LinkedList *l){
  LinkedList *p = l, *q;
  while(p){
    q = p;
    p = p->next;
    free(q);
  } 
}

void linkedListFreeRecursive(LinkedList *l){
  if(l){
    linkedListFreeRecursive(l->next);
    printf("freeing element %d...\n", l->info);
    free(l);
  }
}

LinkedList* linkedListRemove(LinkedList *l, int elem){
  LinkedList *p = l, *ant = NULL;
  while((p) && (p->info != elem)){
    ant = p;
    p = p->next;
  }
  if(!p) return l;
  if(!ant) l = l->next;
  else ant->next = p->next;
  free(p);
  return l;
}

LinkedList* linkedListRemoveRecursive(LinkedList *l, int elem){
  if(!l) return l;
  if(l->info == elem){
    LinkedList *p = l;
    l = l->next;
    free(p);
  }
  else l->next = linkedListRemoveRecursive(l->next, elem);
  return l;
}

LinkedList* linkedListSearch(LinkedList *l, int elem){
  LinkedList *p = l;
  while((p) && (p->info != elem)) p = p->next; 
  return p;
}

LinkedList* linkedListSearchRecursive(LinkedList *l, int elem){
  if((!l) || (l->info == elem)) return l;
  return linkedListSearchRecursive(l->next, elem);
}
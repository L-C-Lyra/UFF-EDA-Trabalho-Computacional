#include <stdio.h>
#include <stdlib.h>

typedef struct listNode{
  int info;
  struct listNode *next;
}LinkedList;

LinkedList* linkedListInitialize(void);
LinkedList* linkedListInsert(LinkedList *l, int elem);
void linkedListInsertVoid(LinkedList **l, int elem);

void linkedListPrint(LinkedList *l);
void linkedListFree(LinkedList *l);
LinkedList* linkedListRemove(LinkedList *l, int elem);
LinkedList* linkedListSearch(LinkedList *l, int elem);

void linkedListPrintRecursive(LinkedList *l);
void linkedListPrintRecursiveReverse(LinkedList *l);
void linkedListFreeRecursive(LinkedList *l);
LinkedList* linkedListSearchRecursive(LinkedList *l, int elem);
LinkedList* linkedListRemoveRecursive(LinkedList *l, int elem);
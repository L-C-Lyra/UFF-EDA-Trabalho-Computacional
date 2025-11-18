#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct listNode{
  void *info;
  struct listNode *next;
}LinkedList;

//LEMBRAR!!!! deve criar as funções para o tipo apropriado caso um novo seja adicionado!
//eu criei uma pequena "biblioteca" para os tipos basicos
typedef int (*CompareFunc)(void*, void*);
typedef void (*PrintFunc)(void*);
typedef void (*FreeFunc)(void*);

LinkedList* linkedListInitialize(void);
LinkedList* linkedListInsert(LinkedList *l, void *elem);
void linkedListInsertVoid(LinkedList **l, void *elem);

void linkedListPrint(LinkedList *l, PrintFunc print);
void linkedListFree(LinkedList *l, FreeFunc freeData);
LinkedList* linkedListRemove(LinkedList *l, void *elem, CompareFunc compare, FreeFunc freeData);
LinkedList* linkedListSearch(LinkedList *l, void *elem, CompareFunc compare);

void linkedListPrintRecursive(LinkedList *l, PrintFunc print);
void linkedListPrintRecursiveReverse(LinkedList *l, PrintFunc print);
void linkedListFreeRecursive(LinkedList *l, FreeFunc freeData);
LinkedList* linkedListSearchRecursive(LinkedList *l, void *elem, CompareFunc compare);
LinkedList* linkedListRemoveRecursive(LinkedList *l, void *elem, CompareFunc compare, FreeFunc freeData);
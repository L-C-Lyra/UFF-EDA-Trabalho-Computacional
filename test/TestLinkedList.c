#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../libs/LinkedList.h"
#include "../libs/GenericImplementations.h"
#include "Tester.h"

//AVISO: os testes não foram gerados com IA mas IA foi usado para formatar os textos de uma forma mais coesa!(sou pessimo escrevendo)
//AVISO2: os testes da linked list apenas usam o tipo int, se necessario adione para outros tipos!

void testLinkedListInitialize() {
    LinkedList* list = linkedListInitialize();
    runTest("linkedListInitialize", list == NULL);
}

void testLinkedListInsertInt() {
    LinkedList* list = linkedListInitialize();
    
    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 30;
    
    list = linkedListInsert(list, a);
    runTest("linkedListInsert - first int element", list != NULL && *(int*)list->info == 10);
    
    list = linkedListInsert(list, b);
    runTest("linkedListInsert - second int element", *(int*)list->info == 20 && *(int*)list->next->info == 10);
    
    list = linkedListInsert(list, c);
    runTest("linkedListInsert - third int element", *(int*)list->info == 30);
    
    linkedListFree(list, freeInt);
}

void testLinkedListInsertString() {
    LinkedList* list = linkedListInitialize();
    
    char *s1 = strdup("hello");
    char *s2 = strdup("world");
    
    list = linkedListInsert(list, s1);
    runTest("linkedListInsert - first string element", list != NULL && strcmp((char*)list->info, "hello") == 0);
    
    list = linkedListInsert(list, s2);
    runTest("linkedListInsert - second string element", strcmp((char*)list->info, "world") == 0);
    
    linkedListFree(list, freeString);
}

void testLinkedListInsertFloat() {
    LinkedList* list = linkedListInitialize();
    
    float *f1 = malloc(sizeof(float)); *f1 = 3.14f;
    float *f2 = malloc(sizeof(float)); *f2 = 2.71f;
    
    list = linkedListInsert(list, f1);
    list = linkedListInsert(list, f2);
    
    runTest("linkedListInsert - float elements", *(float*)list->info == 2.71f);
    
    linkedListFree(list, freeFloat);
}

void testLinkedListInsertVoid() {
    LinkedList* list = linkedListInitialize();
    
    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    
    linkedListInsertVoid(&list, a);
    runTest("linkedListInsertVoid - first element", list != NULL && *(int*)list->info == 10);
    
    linkedListInsertVoid(&list, b);
    runTest("linkedListInsertVoid - second element", *(int*)list->info == 20 && *(int*)list->next->info == 10);
    
    linkedListFree(list, freeInt);
}

void testLinkedListSearch() {
    LinkedList* list = linkedListInitialize();
    
    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 30;
    int search20 = 20, search99 = 99;
    
    list = linkedListInsert(list, a);
    list = linkedListInsert(list, b);
    list = linkedListInsert(list, c);
    
    LinkedList* found = linkedListSearch(list, &search20, compareInt);
    runTest("linkedListSearch - existing element", found != NULL && *(int*)found->info == 20);
    
    found = linkedListSearch(list, &search99, compareInt);
    runTest("linkedListSearch - non-existing element", found == NULL);
    
    found = linkedListSearch(NULL, &search20, compareInt);
    runTest("linkedListSearch - empty list", found == NULL);
    
    linkedListFree(list, freeInt);
}

void testLinkedListSearchRecursive() {
    LinkedList* list = linkedListInitialize();
    
    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 30;
    int search10 = 10, search99 = 99;
    
    list = linkedListInsert(list, a);
    list = linkedListInsert(list, b);
    list = linkedListInsert(list, c);
    
    LinkedList* found = linkedListSearchRecursive(list, &search10, compareInt);
    runTest("linkedListSearchRecursive - existing element", found != NULL && *(int*)found->info == 10);
    
    found = linkedListSearchRecursive(list, &search99, compareInt);
    runTest("linkedListSearchRecursive - non-existing element", found == NULL);
    
    linkedListFree(list, freeInt);
}

void testLinkedListRemove() {
    LinkedList* list = linkedListInitialize();
    
    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 30;
    int search20 = 20, search30 = 30, search99 = 99;
    
    list = linkedListInsert(list, a);
    list = linkedListInsert(list, b);
    list = linkedListInsert(list, c);
    
    list = linkedListRemove(list, &search20, compareInt, freeInt);
    LinkedList* found = linkedListSearch(list, &search20, compareInt);
    runTest("linkedListRemove - middle element", found == NULL);
    
    list = linkedListRemove(list, &search30, compareInt, freeInt);
    runTest("linkedListRemove - first element", *(int*)list->info == 10);
    
    list = linkedListRemove(list, &search99, compareInt, freeInt);
    runTest("linkedListRemove - non-existing element", list != NULL);
    
    linkedListFree(list, freeInt);
}

void testLinkedListRemoveRecursive() {
    LinkedList* list = linkedListInitialize();
    
    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 30;
    int search10 = 10, search99 = 99;
    
    list = linkedListInsert(list, a);
    list = linkedListInsert(list, b);
    list = linkedListInsert(list, c);
    
    list = linkedListRemoveRecursive(list, &search10, compareInt, freeInt);
    LinkedList* found = linkedListSearchRecursive(list, &search10, compareInt);
    runTest("linkedListRemoveRecursive - last element", found == NULL);
    
    list = linkedListRemoveRecursive(list, &search99, compareInt, freeInt);
    runTest("linkedListRemoveRecursive - non-existing element", list != NULL);
    
    linkedListFree(list, freeInt);
}

void testLinkedListPrint() {
    LinkedList* list = linkedListInitialize();
    
    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 30;
    
    list = linkedListInsert(list, a);
    list = linkedListInsert(list, b);
    list = linkedListInsert(list, c);
    
    printf("\nTesting print functions (visual verification required):\n");
    printf("Normal: ");
    linkedListPrint(list, printInt);
    printf("\n");
    
    printf("Recursive: ");
    linkedListPrintRecursive(list, printInt);
    printf("\n");
    
    printf("Recursive Reverse: ");
    linkedListPrintRecursiveReverse(list, printInt);
    printf("\n");
    
    runTest("linkedListPrint functions", 1);
    
    linkedListFree(list, freeInt);
}

int main() {
    printf("Running LinkedList Tests...\n");
    printf("=============================\n");
    testLinkedListInitialize();
    testLinkedListInsertInt();
    testLinkedListInsertString();
    testLinkedListInsertFloat();
    testLinkedListInsertVoid();
    testLinkedListSearch();
    testLinkedListSearchRecursive();
    testLinkedListRemove();
    testLinkedListRemoveRecursive();
    testLinkedListPrint();
    


    printResults();
    return (testsPassed == testsTotal) ? 0 : 1;
}
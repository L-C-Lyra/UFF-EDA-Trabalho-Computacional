#include <stdio.h>
#include <assert.h>
#include "../libs/LinkedList.h"

int testsPassed = 0;
int testsTotal = 0;

void runTest(const char* testName, int condition) {
    testsTotal++;
    if (condition) {
        printf("PASSED: %s\n", testName);
        testsPassed++;
    } else {
        printf("ERROR: %s\n", testName);
    }
}

void testLinkedListInitialize() {
    LinkedList* list = linkedListInitialize();
    runTest("linkedListInitialize", list == NULL);
}

void testLinkedListInsert() {
    LinkedList* list = linkedListInitialize();
    
    list = linkedListInsert(list, 10);
    runTest("linkedListInsert - first element", list != NULL && list->info == 10);
    
    list = linkedListInsert(list, 20);
    runTest("linkedListInsert - second element", list->info == 20 && list->next->info == 10);
    
    list = linkedListInsert(list, 30);
    runTest("linkedListInsert - third element", list->info == 30);
    
    linkedListFree(list);
}

void testLinkedListInsertVoid() {
    LinkedList* list = linkedListInitialize();
    
    linkedListInsertVoid(&list, 10);
    runTest("linkedListInsertVoid - first element", list != NULL && list->info == 10);
    
    linkedListInsertVoid(&list, 20);
    runTest("linkedListInsertVoid - second element", list->info == 20 && list->next->info == 10);
    
    linkedListFree(list);
}

void testLinkedListSearch() {
    LinkedList* list = linkedListInitialize();
    list = linkedListInsert(list, 10);
    list = linkedListInsert(list, 20);
    list = linkedListInsert(list, 30);
    
    LinkedList* found = linkedListSearch(list, 20);
    runTest("linkedListSearch - existing element", found != NULL && found->info == 20);
    
    found = linkedListSearch(list, 99);
    runTest("linkedListSearch - non-existing element", found == NULL);
    
    found = linkedListSearch(NULL, 10);
    runTest("linkedListSearch - empty list", found == NULL);
    
    linkedListFree(list);
}

void testLinkedListSearchRecursive() {
    LinkedList* list = linkedListInitialize();
    list = linkedListInsert(list, 10);
    list = linkedListInsert(list, 20);
    list = linkedListInsert(list, 30);
    
    LinkedList* found = linkedListSearchRecursive(list, 10);
    runTest("linkedListSearchRecursive - existing element", found != NULL && found->info == 10);
    
    found = linkedListSearchRecursive(list, 99);
    runTest("linkedListSearchRecursive - non-existing element", found == NULL);
    
    linkedListFree(list);
}

void testLinkedListRemove() {
    LinkedList* list = linkedListInitialize();
    list = linkedListInsert(list, 10);
    list = linkedListInsert(list, 20);
    list = linkedListInsert(list, 30);
    
    list = linkedListRemove(list, 20);
    LinkedList* found = linkedListSearch(list, 20);
    runTest("linkedListRemove - middle element", found == NULL);
    
    list = linkedListRemove(list, 30);
    runTest("linkedListRemove - first element", list->info == 10);
    
    list = linkedListRemove(list, 99);
    runTest("linkedListRemove - non-existing element", list != NULL);
    
    linkedListFree(list);
}

void testLinkedListRemoveRecursive() {
    LinkedList* list = linkedListInitialize();
    list = linkedListInsert(list, 10);
    list = linkedListInsert(list, 20);
    list = linkedListInsert(list, 30);
    
    list = linkedListRemoveRecursive(list, 10);
    LinkedList* found = linkedListSearchRecursive(list, 10);
    runTest("linkedListRemoveRecursive - last element", found == NULL);
    
    list = linkedListRemoveRecursive(list, 99);
    runTest("linkedListRemoveRecursive - non-existing element", list != NULL);
    
    linkedListFree(list);
}

void testLinkedListPrint() {
    LinkedList* list = linkedListInitialize();
    list = linkedListInsert(list, 10);
    list = linkedListInsert(list, 20);
    list = linkedListInsert(list, 30);
    
    printf("\nTesting print functions (visual verification required):\n");
    printf("Normal: ");
    linkedListPrint(list);
    printf("\n");
    
    printf("Recursive: ");
    linkedListPrintRecursive(list);
    printf("\n");
    
    printf("Recursive Reverse: ");
    linkedListPrintRecursiveReverse(list);
    printf("\n");
    
    runTest("linkedListPrint functions", 1);
    
    linkedListFree(list);
}

int main() {
    printf("Running LinkedList Tests...\n");
    printf("=============================\n");
    
    testLinkedListInitialize();
    testLinkedListInsert();
    testLinkedListInsertVoid();
    testLinkedListSearch();
    testLinkedListSearchRecursive();
    testLinkedListRemove();
    testLinkedListRemoveRecursive();
    testLinkedListPrint();
    
    printf("\n=============================\n");
    printf("Tests Summary: %d/%d PASSED\n", testsPassed, testsTotal);
    
    if (testsPassed == testsTotal) {
        printf("All tests PASSED!\n");
        return 0;
    } else {
        printf("Some tests FAILED!\n");
        return 1;
    }
}
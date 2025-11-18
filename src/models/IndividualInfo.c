#include "IndividualInfo.h"
#include <string.h>
#include <stdio.h>


int compareIndividualInfo(void *a, void *b) {
	IndividualInfo *ia = (IndividualInfo*)a;
	IndividualInfo *ib = (IndividualInfo*)b;
	return strcmp(ia->name, ib->name);
}

void printIndividualInfo(void *data) {
	IndividualInfo *i = (IndividualInfo*)data;
    printf("{\n");
	printf("Individual: %s\n", i->name);
	printf(" - Awards:\n");
	linkedListPrint(i->awardList, printEstandarteAward);
	printf(" - Titles:\n");
	linkedListPrint(i->titleList, printChampionshipInfo);
    printf("}\n");
}
void freeIndividualInfo(void *data) {
	IndividualInfo *i = (IndividualInfo*)data;
	free(i->name);
	linkedListFree(i->awardList, freeEstandarteAward);
	linkedListFree(i->titleList, freeChampionshipInfo);
	free(i);
}

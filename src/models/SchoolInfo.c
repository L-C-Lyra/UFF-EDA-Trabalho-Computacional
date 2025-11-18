#include "SchoolInfo.h"
#include <string.h>

int compareSchoolInfo(void *a, void *b) {
	SchoolInfo *sa = (SchoolInfo*)a;
	SchoolInfo *sb = (SchoolInfo*)b;
	return strcmp(sa->schoolName, sb->schoolName);
}

void printSchoolInfo(void *data) {
	SchoolInfo *s = (SchoolInfo*)data;
    printf("{\n");
	printf("School: %s\n", s->schoolName);
	printf(" - Titles:\n");
	linkedListPrint(s->titleList, printChampionshipInfo);
	printf(" - Runner-ups:\n");
	linkedListPrint(s->runnerUpList, printChampionshipInfo);
	printf("  -Estandarte Awards:\n");
	linkedListPrint(s->estandarteAwardList, printEstandarteAward);
    printf("}\n");
}

void freeSchoolInfo(void *data) {
	SchoolInfo *s = (SchoolInfo*)data;
	free(s->schoolName);
	linkedListFree(s->titleList, freeChampionshipInfo);
	linkedListFree(s->runnerUpList, freeChampionshipInfo);
	linkedListFree(s->estandarteAwardList, freeEstandarteAward);
	free(s);
}

#include "EstandarteAward.h"
#include <string.h>

int compareEstandarteAward(void *a, void *b) {
	EstandarteAward *ea = (EstandarteAward*)a;
	EstandarteAward *eb = (EstandarteAward*)b;
	int yearDiff = ea->year - eb->year;
	if (yearDiff != 0) return yearDiff;
	return strcmp(ea->category, eb->category);
}

void printEstandarteAward(void *data) {
	EstandarteAward *e = (EstandarteAward*)data;
	printf("{Year: %d, Category: %s, Winner: %s}\n", 
		e->year, e->category, e->winner);
}

void freeEstandarteAward(void *data) {
	EstandarteAward *e = (EstandarteAward*)data;
	free(e->category);
	free(e->winner);
	free(e);
}

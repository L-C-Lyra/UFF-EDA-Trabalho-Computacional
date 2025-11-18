#ifndef SCHOOL_INFO_H //LEMBRAR DOS INCLUDE GUARDS!!!!
#define SCHOOL_INFO_H

#include <stdlib.h>
#include "../../libs/LinkedList.h"
#include "ChampionshipInfo.h"
#include "EstandarteAward.h"

typedef struct {
	char *schoolName;
	LinkedList *titleList;
	LinkedList *runnerUpList;
	LinkedList *estandarteAwardList;
} SchoolInfo;

int compareSchoolInfo(void *a, void *b);
void printSchoolInfo(void *data);
void freeSchoolInfo(void *data);

#endif

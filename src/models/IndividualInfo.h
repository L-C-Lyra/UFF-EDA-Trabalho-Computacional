#ifndef INDIVIDUAL_INFO_H
#define INDIVIDUAL_INFO_H
#include <stdlib.h>
#include "../../libs/LinkedList.h"
#include "ChampionshipInfo.h"
#include "EstandarteAward.h"

typedef struct {
	char *name;
	LinkedList *awardList;
	LinkedList *titleList;
} IndividualInfo;

int compareIndividualInfo(void *a, void *b);
void printIndividualInfo(void *data);
void freeIndividualInfo(void *data);



#endif

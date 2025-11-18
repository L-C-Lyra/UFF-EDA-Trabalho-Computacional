#ifndef ESTANDARTE_AWARD_H //LEMBRAR DOS INCLUDE GUARDS!!!!
#define ESTANDARTE_AWARD_H

#include <stdlib.h>
#include "../../libs/LinkedList.h"

typedef struct {
	int year;
	char *category;
	char *winner;
} EstandarteAward;

int compareEstandarteAward(void *a, void *b);
void printEstandarteAward(void *data);
void freeEstandarteAward(void *data);

#endif

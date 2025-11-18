#ifndef CHAMPIONSHIP_INFO_H //LEMBRAR DOS INCLUDE GUARDS!!!!
#define CHAMPIONSHIP_INFO_H //se nao lembrar voce vai 100% de certeza cair em algum erro de compilação!

#include <stdlib.h>
#include "../../libs/LinkedList.h"

typedef struct {
	int year;
	int number;
	char *theme;
	char *carnivalDesigner;
	char *runnerUp;
} ChampionshipInfo;

int compareChampionshipInfo(void *a, void *b);
void printChampionshipInfo(void *data);
void freeChampionshipInfo(void *data);

#endif

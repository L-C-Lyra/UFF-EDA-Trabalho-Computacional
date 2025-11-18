#include "ChampionshipInfo.h"
#include <string.h>

int compareChampionshipInfo(void *a, void *b) {
	ChampionshipInfo *ca = (ChampionshipInfo*)a;
	ChampionshipInfo *cb = (ChampionshipInfo*)b;
	return ca->year - cb->year;
}

void printChampionshipInfo(void *data) {
	ChampionshipInfo *c = (ChampionshipInfo*)data;
	printf("{Year: %d, Number: %d, Theme: %s, Designer: %s, Runner-up: %s}\n", 
		c->year, c->number, c->theme, c->carnivalDesigner, c->runnerUp);
}

void freeChampionshipInfo(void *data) {
	ChampionshipInfo *c = (ChampionshipInfo*)data;
	free(c->theme);
	free(c->carnivalDesigner);
	free(c->runnerUp);
	free(c);
}

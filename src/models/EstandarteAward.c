#include "EstandarteAward.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


EstandarteAward* estandarteAwardCreate(uint16_t year, const char* category, const char* winner) {
    EstandarteAward* award = (EstandarteAward*)malloc(sizeof(EstandarteAward));
    if (!award) return NULL;
    
    award->year = year;
    strncpy(award->category, category, 127);
    award->category[127] = '\0';
    strncpy(award->winner, winner, 255);
    award->winner[255] = '\0';
    
    return award;
}

void estandarteAwardFree(EstandarteAward* award) {
    if (award) free(award);

}

void estandarteAwardPrint(EstandarteAward* award) {
    if (award)
        printf("%d - %s: %s\n", award->year, award->category, award->winner);
}
int estandarteAwardCompare(EstandarteAward* a, EstandarteAward* b) {
    if (!a || !b) return 0;
    int yearDiff = (int)a->year - (int)b->year;
    if (yearDiff != 0) return yearDiff;
    return strcmp(a->category, b->category);
}

#ifndef ESTANDARTE_AWARD_H
#define ESTANDARTE_AWARD_H
#include <stdint.h>

typedef struct {
    uint16_t year;
    char category[128];
    char winner[256];
} EstandarteAward;


EstandarteAward* estandarteAwardCreate(uint16_t year, const char* category, const char* winner);
void estandarteAwardFree(EstandarteAward* award);
void estandarteAwardPrint(EstandarteAward* award);
int estandarteAwardCompare(EstandarteAward* a, EstandarteAward* b);

#endif

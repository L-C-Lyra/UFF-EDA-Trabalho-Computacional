#ifndef PARTICIPATION_H
#define PARTICIPATION_H

#include <stdint.h>

typedef struct {
    char schoolName[256];
    char category[128];
    uint16_t year;
} Participation;

Participation* participationCreate(const char* schoolName, const char* category, uint16_t year);
void participationFree(Participation* participation);
void participationPrint(Participation* participation);
int participationCompare(Participation* a, Participation* b);

#endif

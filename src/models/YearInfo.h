#ifndef YEAR_INFO_H
#define YEAR_INFO_H

#include <stdint.h>

typedef struct {
    uint16_t year;
    char champion[256];
    char runnerUp[256];
    uint8_t titleNumber;
    char theme[256];
    char carnivalDesigner[256];
} YearInfo;

YearInfo* yearInfoCreate(uint16_t year, const char* champion, const char* runnerUp, uint8_t titleNumber, const char* theme, const char* carnivalDesigner);
void yearInfoFree(YearInfo* info);
void yearInfoPrint(YearInfo* info);
int yearInfoCompare(YearInfo* a, YearInfo* b);

#endif

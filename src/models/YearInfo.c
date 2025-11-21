#include "YearInfo.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>



YearInfo* yearInfoCreate(uint16_t year, const char* champion, const char* runnerUp, uint8_t titleNumber, const char* theme, const char* carnivalDesigner) {
    YearInfo* info = (YearInfo*)malloc(sizeof(YearInfo));
    if (!info) return NULL;
    
    info->year = year;
    info->titleNumber = titleNumber;

    strncpy(info->champion, champion, 255);
    info->champion[255] = '\0';
    strncpy(info->runnerUp, runnerUp, 255);
    info->runnerUp[255] = '\0';
    strncpy(info->theme, theme, 255);
    info->theme[255] = '\0';
    
    strncpy(info->carnivalDesigner, carnivalDesigner, 255);
    info->carnivalDesigner[255] = '\0';
    
    return info;
}
void yearInfoFree(YearInfo* info) {
    if (info) free(info);
}
void yearInfoPrint(YearInfo* info) {
    if (info) {
        printf("\n=== carnaval %d ===\n", info->year);
        printf("campea: %s (%dº título)\n", info->champion, info->titleNumber);
        printf("vice campea: %s\n", info->runnerUp);
        printf("enredo: %s\n", info->theme);
        printf("carnavalesco: %s\n", info->carnivalDesigner);
    }
}
int yearInfoCompare(YearInfo* a, YearInfo* b) {
    if (!a || !b) return 0;
    return (int)a->year - (int)b->year;
}

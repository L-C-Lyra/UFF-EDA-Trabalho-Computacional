#include "SchoolInfo.h"
#include "ChampionshipInfo.h"
#include "EstandarteAward.h"
#include "../../libs/GenericTypes.h"
#include "../../libs/GenericImplementations.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

SchoolInfo* schoolInfoCreate(const char* schoolName, uint8_t isActive) {
    SchoolInfo* info = (SchoolInfo*)malloc(sizeof(SchoolInfo));
    if (!info) return NULL;
    
    strncpy(info->schoolName, schoolName, 255);
    info->schoolName[255] = '\0';
    info->isActive = isActive;
    
    info->titleList = linkedListInitialize();
    info->runnerUpList = linkedListInitialize();
    info->awardList = linkedListInitialize();
    
    return info;
}

void schoolInfoFree(SchoolInfo* info) {
    if (info) {
        linkedListFree(info->titleList, (FreeFunc)championshipInfoFree);
        linkedListFree(info->runnerUpList, (FreeFunc)championshipInfoFree);
        linkedListFree(info->awardList, (FreeFunc)estandarteAwardFree);
        free(info);
    }
}

void schoolInfoPrint(SchoolInfo* info) {
    if (!info) return;
    
    printf("\n=== %s %s ===\n", info->schoolName, info->isActive ? "" : "[EXTINTA]"); //amo ternario
    
    int titleCount = linkedListSize(info->titleList);
    int runnerUpCount = linkedListSize(info->runnerUpList);
    int awardCount = linkedListSize(info->awardList);
    
    printf("titulos: %d\n", titleCount);
    if (titleCount > 0) {
        linkedListPrint(info->titleList, (PrintFunc)championshipInfoPrint);
    }
    
    printf("\nvices: %d\n", runnerUpCount);
    if (runnerUpCount > 0) {
        linkedListPrint(info->runnerUpList, (PrintFunc)championshipInfoPrint);
    }
    
    printf("\mestandarte de ouro: %d\n", awardCount);
    if (awardCount > 0) {
        linkedListPrint(info->awardList, (PrintFunc)estandarteAwardPrint);
    }
    
    printf("\n");
}

int schoolInfoCompare(SchoolInfo* a, SchoolInfo* b) {
    if (!a || !b) return 0;
    return strcmp(a->schoolName, b->schoolName);
}
void schoolInfoAddTitle(SchoolInfo* info, void* championshipInfo) {
    
    if (info && championshipInfo) info->titleList = linkedListInsert(info->titleList, championshipInfo);
}
void schoolInfoAddRunnerUp(SchoolInfo* info, void* championshipInfo) {
    
    if (info && championshipInfo)
        info->runnerUpList = linkedListInsert(info->runnerUpList, championshipInfo);
}
void schoolInfoAddAward(SchoolInfo* info, void* estandarteAward) {
   
    if (info && estandarteAward)
        info->awardList = linkedListInsert(info->awardList, estandarteAward);
}

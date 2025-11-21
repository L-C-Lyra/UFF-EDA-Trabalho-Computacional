#include "IndividualInfo.h"
#include "Participation.h"
#include "../../libs/GenericTypes.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

IndividualInfo* individualInfoCreate(const char* personName) {
    IndividualInfo* info = (IndividualInfo*)malloc(sizeof(IndividualInfo));
    if (!info) return NULL;
    
    strncpy(info->personName, personName, 255);
    info->personName[255] = '\0';
    
    info->participationList = linkedListInitialize();
    
    return info;
}

void individualInfoFree(IndividualInfo* info) {
    if (info) {
        linkedListFree(info->participationList, (FreeFunc)participationFree);
        free(info);
    }
}

void individualInfoPrint(IndividualInfo* info) {
    if (!info) return;
    
    int count = linkedListSize(info->participationList);
    printf("\n=== %s ===\n", info->personName);
    printf("total de participações: %d\n", count);
    
    if (count > 0) {
        printf("\participações:\n");
        linkedListPrint(info->participationList, (PrintFunc)participationPrint);
    }
}

int individualInfoCompare(IndividualInfo* a, IndividualInfo* b) {
    if (!a || !b) return 0;
    return strcmp(a->personName, b->personName);
}

void individualInfoAddParticipation(IndividualInfo* info, void* participation) {
    if (info && participation) {
        info->participationList = linkedListInsert(info->participationList, participation);
    }
}

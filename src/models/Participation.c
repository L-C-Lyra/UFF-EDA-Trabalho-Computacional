#include "Participation.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


Participation* participationCreate(const char* schoolName, const char* category, uint16_t year) {
    Participation* participation = (Participation*)malloc(sizeof(Participation));
    if (!participation) return NULL;
    
    strncpy(participation->schoolName, schoolName, 255);
    participation->schoolName[255] = '\0';
    strncpy(participation->category, category, 127);
    participation->category[127] = '\0';
    
    participation->year = year;
    return participation;
}
void participationFree(Participation* participation) {
    if (participation)free(participation);
    
}

void participationPrint(Participation* participation) {
    if (participation)
    printf("%d - %s (%s)\n", participation->year, participation->category, participation->schoolName);
    
}
int participationCompare(Participation* a, Participation* b) {
    if (!a || !b) return 0;
    int yearDiff = (int)a->year - (int)b->year;
    if (yearDiff != 0) return yearDiff;
    return strcmp(a->category, b->category);
}

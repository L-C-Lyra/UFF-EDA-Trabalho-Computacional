#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//byte
int compareByte(void *a, void *b) {
	return *(unsigned char*)a - *(unsigned char*)b;
}
void printByte(void *data) {
	printf("%u ", *(unsigned char*)data);
}
void freeByte(void *data) {
	free((unsigned char*)data);
}

//short
int compareShort(void *a, void *b) {
	return *(short*)a - *(short*)b;
}
void printShort(void *data) {
	printf("%hd ", *(short*)data);
}
void freeShort(void *data) {
	free((short*)data);
}
//int
int compareInt(void *a, void *b) {
	return *(int*)a - *(int*)b;
}
void printInt(void *data) {
	printf("%d ", *(int*)data);
}
void freeInt(void *data) {
	free((int*)data);
}

//long
int compareLong(void *a, void *b) {
	long diff = *(long*)a - *(long*)b;
	if (diff < 0) return -1;
	if (diff > 0) return 1;
	return 0;
}
void printLong(void *data) {
	printf("%ld ", *(long*)data);
}
void freeLong(void *data) {
	free((long*)data);
}


//flt
int compareFloat(void *a, void *b) {
	float diff = *(float*)a - *(float*)b;
	if (diff < 0) return -1;
	if (diff > 0) return 1;
	return 0;
}
void printFloat(void *data) {
	printf("%.2f ", *(float*)data);
}
void freeFloat(void *data) {
	free((float*)data);
}

//double
int compareDouble(void *a, void *b) {
	double diff = *(double*)a - *(double*)b;
	if (diff < 0) return -1;
	if (diff > 0) return 1;
	return 0;
}
void printDouble(void *data) {
	printf("%.2lf ", *(double*)data);
}
void freeDouble(void *data) {
	free((double*)data);
}

//chr
int compareChar(void *a, void *b) {
	return *(char*)a - *(char*)b;
}
void printChar(void *data) {
	printf("%c ", *(char*)data);
}
void freeChar(void *data) {
	free((char*)data);
}
//str
int compareString(void *a, void *b) {
	return strcmp((char*)a, (char*)b);
}
void printString(void *data) {
	printf("%s ", (char*)data);
}
void freeString(void *data) {
	free((char*)data);
}

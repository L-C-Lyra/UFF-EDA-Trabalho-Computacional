#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "LinkedList.h"
#include "HashUtils.h"

typedef struct {
	char *key;
	void *value;
} HashMapEntry;

typedef struct {
	LinkedList **buckets; //nomemclatura (e implementação) inspirada em uma implementação que vi no stackoverflow
	int size;
} HashMap;

//a FreeFunc PrintGunc e etc é a mesma usada no linked list, por isso movi elas para um "generics"
HashMap* hashMapCreate();
void hashMapInsert(HashMap *map, char *key, void *value);
void* hashMapGet(HashMap *map, char *key);
bool hashMapContains(HashMap *map, char *key);
void* hashMapRemove(HashMap *map, char *key, FreeFunc freeValue);
void hashMapFree(HashMap *map, FreeFunc freeValue);
void hashMapPrint(HashMap *map, PrintFunc printValue);

int compareHashMapEntry(void *a, void *b);
void freeHashMapEntry(void *data);

#endif

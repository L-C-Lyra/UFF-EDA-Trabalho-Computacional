#include "HashMap.h"
#include <string.h>

HashMap* hashMapCreate() {
	HashMap *map = (HashMap*)malloc(sizeof(HashMap));
	map->size = DIVIDER;
	map->buckets = (LinkedList**)calloc(DIVIDER, sizeof(LinkedList*)); //https://pt.stackoverflow.com/questions/179205/qual-%C3%A9-a-diferen%C3%A7a-entre-calloc-e-malloc
	for (int i = 0; i < DIVIDER; i++) {
		map->buckets[i] = linkedListInitialize();
	}
	return map;
}

int compareHashMapEntry(void *a, void *b) {
	HashMapEntry *ea = (HashMapEntry*)a;
	HashMapEntry *eb = (HashMapEntry*)b;
	return strcmp(ea->key, eb->key);
}
void freeHashMapEntry(void *data) {
	HashMapEntry *entry = (HashMapEntry*)data;
	free(entry->key);
	free(entry);
}


void hashMapInsert(HashMap *map, char *key, void *value) {
	int index = hashFromString(key);
	HashMapEntry searchEntry;
	searchEntry.key = key;
	LinkedList *existing = linkedListSearch(map->buckets[index], &searchEntry, compareHashMapEntry);
	
	if(existing){
		HashMapEntry *entry = (HashMapEntry*)existing->info;
		entry->value = value;
	}  else 
    {
		HashMapEntry *newEntry = (HashMapEntry*)malloc(sizeof(HashMapEntry));
		newEntry->key = strdup(key);
		newEntry->value = value;
		map->buckets[index] = linkedListInsert(map->buckets[index], newEntry);
	}
}

void* hashMapGet(HashMap *map, char *key) {
	int index = hashFromString(key);
	HashMapEntry searchEntry;
	searchEntry.key = key;
	LinkedList *found = linkedListSearch(map->buckets[index], &searchEntry, compareHashMapEntry);
	if(found){
		HashMapEntry *entry = (HashMapEntry*)found->info;
		return entry->value;
	}
	
	return NULL;
}

bool hashMapContains(HashMap *map, char *key) {
	return hashMapGet(map, key) != NULL;
}

void* hashMapRemove(HashMap *map, char *key, FreeFunc freeValue) {
	int index = hashFromString(key);
	
	HashMapEntry searchEntry;
	searchEntry.key = key;
	
	LinkedList *found = linkedListSearch(map->buckets[index], &searchEntry, compareHashMapEntry);
	
	if (found){
		HashMapEntry *entry = (HashMapEntry*)found->info;
		void *value = entry->value;
		if (freeValue) freeValue(value);
		
		map->buckets[index] = linkedListRemove(map->buckets[index], &searchEntry, compareHashMapEntry, freeHashMapEntry);
		return value;
	}
	return NULL;
}

void hashMapFree(HashMap *map, FreeFunc freeValue) {
	for (int i = 0; i < map->size; i++) {
		LinkedList *current = map->buckets[i];
		while (current) {
			HashMapEntry *entry = (HashMapEntry*)current->info;
			if (freeValue) {
				freeValue(entry->value);
			}
			current = current->next;
		}
		linkedListFree(map->buckets[i], freeHashMapEntry);
	}
	
	free(map->buckets);
	free(map);
}

void hashMapPrint(HashMap *map, PrintFunc printValue) {
	printf("HashMap {\n");
	for (int i = 0; i < map->size; i++) {
		if (map->buckets[i] != NULL) {
			LinkedList *current = map->buckets[i];
			while (current) {
				HashMapEntry *entry = (HashMapEntry*)current->info;
				printf("  [%d] %s => ", i, entry->key);
				if (printValue) {
					printValue(entry->value);
				} else {
					printf("%p", entry->value);
				}
				printf("\n");
				current = current->next;
			}
		}
	}
	printf("}\n");
}

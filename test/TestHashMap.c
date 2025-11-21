//esse teste ta com memory leak parece mas como nao faz parte do programa principal e nao parece ser na implementação da hash eu nao vou olhar muito

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../libs/HashMap.h"
#include "../libs/GenericImplementations.h"
#include "Tester.h"

void testHashMapCreate() {
	HashMap *map = hashMapCreate();
	runTest("hashMapCreate - map not null", map != NULL);
	runTest("hashMapCreate - size is DIVIDER", map->size == DIVIDER);
	hashMapFree(map, NULL);
}
void testHashMapInsertAndGet() {
	HashMap *map = hashMapCreate();
	
	int *val1 = malloc(sizeof(int)); *val1 = 32; //cria so um tamanho arbitrario pra testar se tudo funciona como esperado
	int *val2 = malloc(sizeof(int)); *val2 = 64;
	hashMapInsert(map, "k1", val1);
	hashMapInsert(map, "k2", val2);
	
	int *retrieved1 = (int*)hashMapGet(map, "k1");
	int *retrieved2 = (int*)hashMapGet(map, "k2");
	
	runTest("hashMapGet - retrieve first value", retrieved1 != NULL && *retrieved1 == 32);
	runTest("hashMapGet - retrieve second value", retrieved2 != NULL && *retrieved2 == 64);
	
	hashMapFree(map, freeInt);
}
void testHashMapContains() {
	HashMap *map = hashMapCreate();
	int *val = malloc(sizeof(int)); *val = 10;
	hashMapInsert(map, "tocomfome", val);
	
	runTest("hashMapContains - existin key", hashMapContains(map, "tocomfome") == true);
	runTest("hashMapContains - nonexisting key", hashMapContains(map, "batata") == false);
	
	hashMapFree(map, freeInt);
}

//se der set no mesmo valor a hash tem que atualizar o valor existente
void testHashMapUpdate() {
	HashMap *map = hashMapCreate();
	
	int *val1 = malloc(sizeof(int)); *val1 = 10;
	int *val2 = malloc(sizeof(int)); *val2 = 20;
	
	hashMapInsert(map, "key", val1);
	hashMapInsert(map, "key", val2);
	
	int *retrieved = (int*)hashMapGet(map, "key");
	runTest("hashMapInsert - update overwrites value", retrieved != NULL && *retrieved == 20);
	free(val1);
	hashMapFree(map, freeInt);
}
void testHashMapCollision() {
	HashMap *map = hashMapCreate();
	
	char *key1 = "abc";
	char *key2 = "def";
	char *key3 = "ghi";
	int *val1 = malloc(sizeof(int)); *val1 = 1;
	int *val2 = malloc(sizeof(int)); *val2 = 2;
	int *val3 = malloc(sizeof(int)); *val3 = 3;
	
	hashMapInsert(map, key1, val1);
	hashMapInsert(map, key2, val2);
	hashMapInsert(map, key3, val3);
	
	int *r1 = (int*)hashMapGet(map, key1);
	int *r2 = (int*)hashMapGet(map, key2);
	int *r3 = (int*)hashMapGet(map, key3);
	
	runTest("Collision handling - retrieve fist", r1 != NULL && *r1 == 1);
	runTest("Collision handling - retrieve second", r2 != NULL && *r2 == 2);
	runTest("Collision handling - retrieve third", r3 != NULL && *r3 == 3);
	
	hashMapFree(map, freeInt);
}

void testHashMapRemove() {
	HashMap *map = hashMapCreate();
	
	int *val1 = malloc(sizeof(int)); *val1 = 10;
	int *val2 = malloc(sizeof(int)); *val2 = 20;
	
	hashMapInsert(map, "key1", val1);
	hashMapInsert(map, "key2", val2);
	
	hashMapRemove(map, "key1", freeInt);
	
	runTest("hashMapRemove - key removed", hashMapContains(map, "key1") == false);
	runTest("hashMapRemove - other key still exists", hashMapContains(map, "key2") == true);
	
	hashMapFree(map, freeInt);
}

void testHashMapWithStrings() {
	HashMap *map = hashMapCreate();
	
	char *escola1 = strdup("Mangueira");
	char *escola2 = strdup("Portela");
	char *escola3 = strdup("Beija-Flor");
	
	hashMapInsert(map, "verde_rosa", escola1);
	hashMapInsert(map, "aguia", escola2);
	hashMapInsert(map, "beija_flor", escola3);
	
	char *r1 = (char*)hashMapGet(map, "verde_rosa");
	char *r2 = (char*)hashMapGet(map, "aguia");
	char *r3 = (char*)hashMapGet(map, "beija_flor");
	
	runTest("HashMap with strings - first", r1 != NULL && strcmp(r1, "Mangueira") == 0);
	runTest("HashMap with strings - second", r2 != NULL && strcmp(r2, "Portela") == 0);
	runTest("HashMap with strings - third", r3 != NULL && strcmp(r3, "Beija-Flor") == 0);
	
	hashMapFree(map, freeString);
}
void testHashMapMultipleCollisions() {
	HashMap *map = hashMapCreate();
	for(int i = 0; i < 10; i++){
		char key[20];
		sprintf(key, "key%d", i);
		int *val = malloc(sizeof(int));
		*val = i * 10;
		hashMapInsert(map, key, val);
	}
	int allFound = 1;
	for(int i = 0; i < 10; i++){

		char key[20];
		sprintf(key, "key%d", i);
		int *val = (int*)hashMapGet(map, key);
		if (!val || *val != i * 10) {
			allFound = 0;
			break;
		}
	}
	
	runTest("Multiple inserctions - all values retrievable", allFound);
	hashMapFree(map, freeInt);
}




int main() {
	printf("Running HashMap Tests...\n");
	printf("========================\n");
	
	testHashMapCreate();
	testHashMapInsertAndGet();
	testHashMapContains();
	testHashMapUpdate();
	testHashMapCollision();
	testHashMapRemove();
	testHashMapWithStrings();
	testHashMapMultipleCollisions();
	
	printResults();
	
	return  (testsPassed == testsTotal) ? 0 :1;
}

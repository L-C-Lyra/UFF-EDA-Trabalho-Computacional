#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../libs/HashUtils.h"
#include "Tester.h"

void testHashFromString() {
	char *str1 = "teste"; //to sem criatividade
	char *str2 = "teste";
	char *str3 = "outro";
	int hash1 = HashFromString(str1);
	int hash2 = HashFromString(str2);
	int hash3 = HashFromString(str3);
	
	runTest("HashFromString - same string same hash", hash1 == hash2);
	runTest("HashFromString - different strings different hash", hash1 != hash3);
	runTest("HashFromString - hash in range", hash1 >= 0 && hash1 < 256);
}

void testHashFromStringEmpty() {
	char *empty = "";
	int hash = HashFromString(empty);
	
	runTest("HashFromString - empty string", hash == 0);
}

void testHashFromStringSingleChar() {
	char *single = "a";
	int hash = HashFromString(single);
	
	runTest("HashFromString - single char", hash == 'a' % 256);
}

void testTestStringHashCollision() {
	char *str1 = "abc";
	char *str2 = "abc";
	char *str3 = "def";
	runTest("TestStringHashCollision - identical strings", TestStringHashCollision(str1, str2) == true);
	runTest("TestStringHashCollision - different strings", TestStringHashCollision(str1, str3) == false);
}

void testHashDistribution() {
	char *strings[] = {
		"Mangueira",
		"Portela",
		"Beija-Flor",
		"Salgueiro",
		"Imperatriz",
		"Vila Isabel",
		"Mocidade", // padre miguel
        "Arranco do Engenho de Dentro" //a melhor, quem discorda ta errado, zilmar conde meu heroi!
	};

	int numStrings = 8;
	int hashes[8];
	int allDifferent = 1;
	
	for (int i = 0; i < numStrings; i++) {
		hashes[i] = HashFromString(strings[i]);
	}
	for (int i = 0; i < numStrings; i++) {
		for (int j = i + 1; j < numStrings; j++) {
			if (hashes[i] == hashes[j]) {
				allDifferent = 0;
				break;
			}
		}
	}
	
	runTest("Hash distribution - school names", allDifferent);
}

int main() {
	printf("Running HashUtils Tests...\n");
	printf("===========================\n");
	
	testHashFromString();
	testHashFromStringEmpty();
	testHashFromStringSingleChar();
	testTestStringHashCollision();
	testHashDistribution();
	
	printResults();
	
	return (testsPassed == testsTotal) ? 0 : 1;
}

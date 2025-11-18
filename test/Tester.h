#include <stdio.h>

int testsPassed = 0;
int testsTotal = 0;

//movido para um header proprio para facilitar a implementação
//USO: passe o nome do test (eg: "1+1=2 test") e a condição (eg: "1 + 1 == 2")
void runTest(const char* testName, int condition) {
	testsTotal++;
	if (condition) {
		printf("PASSED: %s\n", testName);
		testsPassed++;
	} else {
		printf("ERROR: %s\n", testName);
	}
}

//advinha o que isso faz... :p
void printResults() {
	printf("\n=============================\n");
	printf("Tests Summary: %d/%d PASSED\n", testsPassed, testsTotal);
	if (testsPassed == testsTotal) {
		printf("All tests PASSED!\n");
	} else {
		printf("Some tests FAILED!\n");
	}
    printf("=============================\n\n");
}

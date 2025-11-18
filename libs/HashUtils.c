#include "HashUtils.h"

int HashFromString(char *str) {
	int sum = 0;
	int len = strlen(str);
	for (int i = 0; i < len; i++) {
		sum += (unsigned char)str[i];
	}
	return sum % DIVIDER;
}

bool TestStringHashCollision(char *str1, char *str2) {
	int hash1 = HashFromString(str1);
	int hash2 = HashFromString(str2);
	return hash1 == hash2;
}

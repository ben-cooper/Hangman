#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "misc.h"

#define MAX_STRING_SIZE 100

// convert strings to lowercase and remove non alphabetic characters
void sanitize(char *str, size_t len, char const *exceptions)
{
	char buffer[MAX_STRING_SIZE];
	size_t i;
	size_t current = 0;

	// checking string size
	if (len > MAX_STRING_SIZE) {
		fprintf(stderr, "String exceeded maximum size!\n");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < len; i++) {
		if ((isalpha(str[i])) || (index(exceptions, str[i]))){
			buffer[current] = tolower(str[i]);
			current++;
		}
	}

	buffer[current] = '\0';

	strncpy(str, buffer, len);
}

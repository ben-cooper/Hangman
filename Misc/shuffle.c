#include <stdlib.h>
#include <time.h>
#include "misc.h"

// shuffles an array of strings using the fisher-yates shuffle
void shuffle(char **strs, size_t n)
{
	size_t i;
	size_t j;
	char *temp;

	srand(time(NULL));

	for (i = 0; i < (n - 2); i++) {
		j = i + (rand() % (n - i));

		temp = strs[j];
		strs[j] = strs[i];
		strs[i] = temp;
	}
}

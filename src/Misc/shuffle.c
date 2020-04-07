#include <stdlib.h>
#include <time.h>
#include "misc.h"

void shuffle(char **strs, size_t n)
{
	size_t i;
	size_t j;
	char *tmp;

	srand(time(NULL));

	for (i = 0; i < (n - 2); i++) {
		j = i + (rand() % (n - i));

		tmp = strs[j];
		strs[j] = strs[i];
		strs[i] = tmp;
	}
}

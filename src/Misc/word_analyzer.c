#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "misc.h"

struct pair {
	char letter;
	float chance;
};

int pair_compare(void const *x_arg, void const *y_arg)
{
	struct pair const *x = x_arg;
	struct pair const *y = y_arg;

	if (x->chance < y->chance)
		return 1;
	else if (x->chance > y->chance)
		return -1;
	else
		return 0;
}

void print_probability(char *const *words, size_t n, char const *exceptions)
{
	size_t i;
	size_t j;
	size_t len;
	int idx;
	int word_count[26] = { 0 };
	int letter_bitmap[26] = { 0 };

	struct pair results[26];

	/* initializing result */
	for (i = 0; i < 26; i++)
		results[i].letter = 'a' + i;

	for (i = 0; i < n; i++) {
		len = strlen(words[i]);

		/* adding each letter of word */
		for (j = 0; j < len; j++) {
			idx = words[i][j] - 97;
			letter_bitmap[idx] = 1;
		}

		/* adding bitmap to total word count */
		for (j = 0; j < 26; j++)
			word_count[j] += letter_bitmap[j];

		memset(letter_bitmap, 0, sizeof(letter_bitmap));
	}

	for (i = 0; i < 26; i++)
		results[i].chance = (float) word_count[i] / n * 100;

	qsort(results, 26, sizeof(struct pair), pair_compare);

	printf("None\b\b\b\b");

	for (i = 0; i < 26; i++)
		if ((!strchr(exceptions, results[i].letter)) && (results[i].chance))
			printf("Letter: %c\tChance: %.2f%%\n", results[i].letter,
			       results[i].chance);
}

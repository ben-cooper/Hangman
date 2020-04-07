#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "misc.h"

void insertion_sort(double *values, char *keys, size_t len)
{
	size_t i;
	size_t j;
	double tmp_v;
	char tmp_k;

	for (i = 0; i < len; i++) {
		j = i;
		while ((j > 0) && (values[j - 1] < values[j])) {
			/* swapping */
			tmp_v = values[j];
			tmp_k = keys[j];
			values[j] = values[j - 1];
			keys[j] = keys[j - 1];
			values[j - 1] = tmp_v;
			keys[j - 1] = tmp_k;

			j--;
		}
	}
}

void print_probability(char *const *str, size_t n, char const *exceptions)
{
	size_t i;
	size_t j;
	size_t len;
	int idx;
	float chance;

	int word_count[26] = { 0 };
	int letter_bitmap[26] = { 0 };

	/* arrays for sorting result */
	double probability[26] = { 0 };
	char letters[26] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
						'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
						's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

	for (i = 0; i < n; i++) {
		len = strlen(str[i]);

		/* adding each letter of word */
		for (j = 0; j < len; j++) {
			idx = str[i][j] - 97;
			letter_bitmap[idx] = 1;
		}

		/* adding bitmap to total word count */
		for (j = 0; j < 26; j++)
			word_count[j] += letter_bitmap[j];

		memset(letter_bitmap, 0, sizeof(letter_bitmap));
	}

	for (i = 0; i < 26; i++) {
		chance = (double) word_count[i] / n * 100;
		probability[i] = chance;
	}

	insertion_sort(probability, letters, 26);

	for (i = 0; i < 26; i++)
		if ((!index(exceptions, letters[i])) && (probability[i]))
			printf("Letter: %c\tChance: %.2f%%\n", letters[i], probability[i]);
}

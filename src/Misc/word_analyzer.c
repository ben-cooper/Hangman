#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "misc.h"

void insertion_sort(double *values, char *keys, size_t len)
{
	/* indexes */
	size_t i;
	size_t j;
	/* temporary */
	double v;
	char k;

	for (i = 0; i < len; i++) {
		j = i;
		while ((j > 0) && (values[j - 1] < values[j])) {
			/* swapping */
			v = values[j];
			k = keys[j];
			values[j] = values[j - 1];
			keys[j] = keys[j - 1];
			values[j - 1] = v;
			keys[j - 1] = k;

			j--;
		}
	}
}

/* print probabilities of each lettter occuring in a random
 word from the provided list.
 Requires strings to be lower case and contain only
 alphabetic characters */

void print_probabilities(char *const *str, size_t n, char const *exceptions)
{
	size_t i;
	size_t j;
	size_t len;
	int idx;
	float chance;

	/* number of words with each letter */
	int letter_count[26] = { 0 };
	/* bitmap to for the presence of each letter in the word */
	int letter_bitmap[26] = { 0 };

	/* arrays for sorting result */
	double probabilities[26] = { 0 };
	char letters[26] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
		'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
		's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
	};

	/* adding letters of each word */
	for (i = 0; i < n; i++) {
		len = strlen(str[i]);

		/* adding each letter of word */
		for (j = 0; j < len; j++) {
			idx = str[i][j] - 97;
			/* setting letter in bitmap */
			letter_bitmap[idx] = 1;
		}

		/* adding bitmap to total word count */
		for (idx = 0; idx < 26; idx++) {
			letter_count[idx] += letter_bitmap[idx];
		}

		/* resetting letter bitmap */
		memset(letter_bitmap, 0, sizeof(letter_bitmap));
	}

	/* calculating results */
	for (idx = 0; idx < 26; idx++) {
		chance = (double)letter_count[idx] / n * 100;
		probabilities[idx] = chance;
	}

	/* sorting result */
	insertion_sort(probabilities, letters, 26);

	/*  outputting results */
	for (idx = 0; idx < 26; idx++) {
		/* letters are not in exceptions and the probability is not 0 */
		if ((!index(exceptions, letters[idx])) && (probabilities[idx])) {
			printf("Letter: %c\tChance: %.2f%%\n", letters[idx],
			       probabilities[idx]);
		}
	}
}

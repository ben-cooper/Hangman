#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <time.h>
#include "word_utilities.h"

#define PRINT_COL_LIMIT 80
#define PRINT_ROW_LIMIT 15
#define DELIM "  "
#define DELIM_LENGTH 2

static int word_count[26];

void process_word(char const *word, size_t len)
{

	size_t i;
	int idx;
	int letter_bitmap[26] = { 0 };

	/* adding each letter of word */
	for (i = 0; i < len; i++) {
		idx = word[i] - 'a';
		letter_bitmap[idx] = 1;
	}

	/* adding bitmap to total word count */
	for (i = 0; i < 26; i++)
		word_count[i] += letter_bitmap[i];
}


struct pair {
	char letter;
	float chance;
};

static int pair_compare(void const *x_arg, void const *y_arg)
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

void print_probability(size_t words, char const *exceptions)
{
	size_t i;

	struct pair results[26];

	/* initializing result */
	for (i = 0; i < 26; i++) {
		results[i].letter = 'a' + i;
		results[i].chance = (float) word_count[i] / (float) words * 100.00;
	}

	qsort(results, 26, sizeof(struct pair), pair_compare);

	printf("None\b\b\b\b");

	for (i = 0; i < 26; i++)
		if ((!strchr(exceptions, results[i].letter)) && (results[i].chance))
			printf("Letter: %c\tChance: %.2f%%\n", results[i].letter,
			       results[i].chance);

	memset(word_count, 0, sizeof(word_count));
}

int print_next_word(char const *word, size_t len)
{
	static size_t col = 0;
	static size_t row = 0;

	/* printing word */
	if (col > PRINT_COL_LIMIT) {

		if (row < PRINT_ROW_LIMIT) {
			printf("\n");

			col = 0;
			row++;
		}

	} else {
		printf("%s" DELIM, word);
		col += len + DELIM_LENGTH;
	}

	if (row >= PRINT_ROW_LIMIT)
	{
		/* limit reached */
		col = 0;
		row = 0;

		return 1;
	}

	return 0;
}

int sanitized(char const *str, char const *except)
{
	size_t i;
	size_t len = strlen(str);

	/* empty strings are not valid */
	if (!len)
		return 0;

	for (i = 0; i < len; i++)
		if (!((isalpha(str[i]) && islower(str[i])) ||(strchr(except, str[i]))))
			return 0;

	return 1;
}

void shuffle(char **strs, size_t n)
{
	size_t i;
	size_t j;
	char *tmp;

	if (n < 2)
		return;

	srand(time(NULL));

	for (i = 0; i < (n - 2); i++) {
		j = i + (rand() % (n - i));

		tmp = strs[j];
		strs[j] = strs[i];
		strs[i] = tmp;
	}
}

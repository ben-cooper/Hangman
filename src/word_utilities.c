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

static size_t letter_count[26];
static size_t word_count = 0;

static size_t col = 0;
static size_t row = 0;

static int print_limit_reached = 0;

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
		letter_count[i] += letter_bitmap[i];

	word_count++;
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

void print_probability(char const *exceptions)
{
	size_t i;

	struct pair results[26];

	if (word_count == 0)
	{
		printf("Could not find any possible words!\n");
		return;
	}

	printf("Words found: %lu\n\n", word_count);
	printf("Letter probabilities:\n");

	/* initializing result */
	for (i = 0; i < 26; i++) {
		results[i].letter = 'a' + i;
		results[i].chance = (float) letter_count[i] / (float) word_count * 100.00;
	}

	qsort(results, 26, sizeof(struct pair), pair_compare);

	printf("None\b\b\b\b");

	for (i = 0; i < 26; i++)
		if ((!strchr(exceptions, results[i].letter)) && (results[i].chance))
			printf("Letter: %c\tChance: %.2f%%\n", results[i].letter,
			       results[i].chance);

	memset(letter_count, 0, sizeof(letter_count));
}

void reset_word_position()
{
	print_limit_reached = 0;
	col = 0;
	row = 0;
}

void print_next_word(char const *word, size_t len)
{
	if (print_limit_reached == 1)
		return;

	/* printing word */
	if (col > PRINT_COL_LIMIT) {
		col = 0;

		if (row < PRINT_ROW_LIMIT) {
			printf("\n");
			row++;
		} else {
			printf("\n\nPrint Limit reached!\n");

			print_limit_reached = 1;
			row = 0;

			return;
		}
	}

	printf("%s" DELIM, word);
	col += len + DELIM_LENGTH;
}

int sanitized(char const *str, char const *except)
{
	size_t i;
	size_t len = strlen(str);

	if (!len)
		return 1;

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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "word_utilities.h"

static size_t letter_count[26];
static size_t word_count = 0;

static size_t col = 0;
static size_t row = 0;

static bool print_limit_reached = 0;

void process_word(char const *word, size_t len)
{

	size_t i;
	int idx;
	int letter_bitmap[26] = { 0 };

	// adding each letter of word
	for (i = 0; i < len; i++) {
		idx = word[i] - 'a';
		letter_bitmap[idx] = 1;
	}

	// adding bitmap to total word count
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

	if (word_count == 0) {
		printf("Could not find any possible words!\n");
		return;
	}

	printf("Words found: %zu\n\n", word_count);
	printf("Letter probabilities:\n");

	// initializing result
	for (i = 0; i < 26; i++) {
		results[i].letter = 'a' + i;
		results[i].chance =
		    (float)letter_count[i] / (float)word_count *100.00;
	}

	qsort(results, 26, sizeof(struct pair), pair_compare);

	printf("None\b\b\b\b");

	for (i = 0; i < 26; i++)
	        if ((!strchr(exceptions, results[i].letter))
		    && (results[i].chance))
			printf("Letter: %c\tChance: %.2f%%\n",
	                       results[i].letter, results[i].chance);

}

void reset_words(void)
{

	// resetting word counts
	memset(letter_count, 0, sizeof(letter_count));
	word_count = 0;

	// resetting print positions
	print_limit_reached = false;
	col = 0;
	row = 0;
}

void print_next_word(char const *word, size_t len)
{
	if (print_limit_reached == true)
		return;

	// printing word
	if (col > PRINT_COL_LIMIT) {
		col = 0;

		if (row < PRINT_ROW_LIMIT) {
			printf("\n");
			row++;
		} else {
			printf("\n\nPrint Limit reached!\n");

			print_limit_reached = true;
			row = 0;

			return;
		}
	}

	printf("%s" DELIM, word);

	// subtracting 1 for the zero byte
	col += len + sizeof(DELIM) - 1;
}

bool sanitized(char const *str)
{
	while (*str != '\0') {

		if (((*str < 'a') || (*str > 'z')) && (*str != WILDCARD))
			return false;

		str++;
	}

	return true;
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

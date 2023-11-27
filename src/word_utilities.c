#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "word_utilities.h"

void process_word(struct printer *p, char const *word, size_t len)
{

	size_t i;
	size_t j;
	bool letter_bitmap[26] = { false };

	// adding each letter of word
	for (i = 0; i < len; i++) {
		j = word[i] - 'a';
		letter_bitmap[j] = true;
	}

	// adding bitmap to total word count
	for (i = 0; i < 26; i++)
		p->letter_count[i] += letter_bitmap[i];

	p->word_count++;
}

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

void print_probability(struct printer *p, char const *exceptions)
{
	size_t i;

	struct pair results[26];

	if (p->word_count == 0) {
		printf("Could not find any possible words!\n");
		return;
	}

	printf("Words found: %zu\n\n", p->word_count);
	printf("Letter probabilities:\n");

	// initializing result
	for (i = 0; i < 26; i++) {
		results[i].letter = 'a' + i;
		results[i].chance =
		    (float)(p->letter_count[i]) / (float)(p->word_count) * 100.00;
	}

	qsort(results, 26, sizeof(struct pair), pair_compare);

	printf("None\b\b\b\b");

	for (i = 0; i < 26; i++)
	        if ((!strchr(exceptions, results[i].letter))
		    && (results[i].chance))
			printf("Letter: %c\tChance: %.2f%%\n",
	                       results[i].letter, results[i].chance);

}

void print_next_word(struct printer *p, char const *word, size_t len)
{
	if (p->print_limit_reached == true)
		return;

	// printing word
	if (p->col > PRINT_COL_LIMIT) {
		p->col = 0;

		if (p->row < PRINT_ROW_LIMIT) {
			printf("\n");
			p->row++;
		} else {
			printf("\n\nPrint Limit reached!\n");

			p->print_limit_reached = true;
			p->row = 0;

			return;
		}
	}

	printf("%s" DELIM, word);

	// subtracting 1 for the zero byte
	p->col += len + sizeof(DELIM) - 1;
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

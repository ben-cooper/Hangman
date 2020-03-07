#include "misc.h"

/* print probabilities of each lettter occuring in a random
 word from the provided list.
 Requires strings to be lower case and contain only
 alphabetic characters */
 
void print_probabilities(char *const *str, size_t n)
{
	// number of words with each letter
	int letter_count[26]  = { 0 };
	// bitmap to for the presence of each letter in the word
	int letter_bitmap[26] = { 0 };
	
	size_t i;
	size_t j;
	size_t len;
	int idx;
	float chance;
	
	// through each word
	for (i = 0; i < n; i++)
	{
		len = strlen(str[i]);
		
		// through each letter
		for (j = 0; j < len; j++)
		{
			idx = str[i][j] - 97;
			// setting letter in bitmap
			letter_bitmap[idx] = 1;
		}
		
		// adding bitmap to total word count
		for (idx = 0; idx < 26; idx++)
		{
			letter_count[idx] += letter_bitmap[idx];
		}
		
		// resetting letter bitmap
		memset(letter_bitmap, 0, sizeof(letter_bitmap));
	}
	
	// outputting results
	for (idx = 0; idx < 26; idx++)
	{
		chance = (double) letter_count[idx] / n * 100;
		printf("Letter: %c\tChance: %.2f%%\n", idx + 97, chance);
	}
}

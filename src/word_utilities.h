#ifndef MISC_H
#define MISC_H

#include <stdio.h>
#include <stdbool.h>

#define WILDCARD '-'

#define PRINT_COL_LIMIT 80
#define PRINT_ROW_LIMIT 15

#define DELIM "  "

struct pair {
	char letter;
	float chance;
};

struct printer {
	size_t letter_count[26];
	size_t word_count;
	size_t col;
	size_t row;
	bool print_limit_reached;
};

/**
 * Increments global word_count based on the letters present in word
 * @param word: the word to be analyzed
 * @param len: the length of the word
 */
void process_word(struct printer *p, char const *word, size_t len);

/**
 * Prints frequency of each letter in the list of words.  Assumes
 * strings are all lower case letters.  Resets global word_count.
 * @param str: array of string
 * @param n: the length of the array
 * @param exceptions: array of characters to exclude
 */
void print_probability(struct printer *p, char const *exceptions);

/**
 * Prints words in a rectangular pattern defined by PRINT_ROW_LIMIT and
 * PRINT_COL_LIMIT
 * @param word: the word to be printed
 * @param len: the length of the word
 */
void print_next_word(struct printer *p, char const *word, size_t len);

/**
 * Randomizes the string in the array (fisher-yates shuffle)
 * @param strs: array of strings
 * @param n: the length of the array
 */
void shuffle(char **strs, size_t n);

/**
 * Return true if string contains no illegal characters
 * @param str: the input string
 */
bool sanitized(char const *str);

#endif

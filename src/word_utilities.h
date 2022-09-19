#ifndef MISC_H
#define MISC_H

#include <stdio.h>

/**
 * Increments global word_count based on the letters present in word
 * @param word: the word to be analyzed
 * @param len: the length of the word
 */
void process_word(char const *word, size_t len);

/**
 * Prints frequency of each letter in the list of words.  Assumes
 * strings are all lower case letters.  Resets global word_count.
 * @param str: array of string
 * @param n: the length of the array
 * @param exceptions: array of characters to exclude
 */
void print_probability(size_t words, char const *exceptions);

/**
 * Randomizes the string in the array (fisher-yates shuffle)
 * @param strs: array of strings
 * @param n: the length of the array
 */
void shuffle(char **strs, size_t n);

/**
 * Returns 1 if the string contains only alphabetical characters
 * @param str: the input string
 * @param exceptions: array of characters to ignore while sanitizing
 */
int sanitized(char const *str, char const *exceptions);

/**
 * Prints words in a rectangular pattern defined by PRINT_ROW_LIMIT and
 * PRINT_COL_LIMIT
 * @param word: the word to be printed
 * @param len: the length of the word
 * @return 1 if print limit has been reached, 0 otherwise
 */
int print_next_word(char const *word, size_t len);

#endif

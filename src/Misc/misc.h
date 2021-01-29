#ifndef MISC_H
#define MISC_H

/**
 * Prints frequency of each letter in the list of words.  Assumes
 * strings are all lower case letters
 * @param str: array of string
 * @param n: the length of the array
 * @param exceptions: array of characters to exclude
 */
void print_probability(char *const *str, size_t n, char const *exceptions);

/**
 * Randomizes the string in the array (fisher-yates shuffle)
 * @param strs: array of strings
 * @param n: the length of the array
 */
void shuffle(char **strs, size_t n);

/**
 * Returns 1 if the string contains only alphabetical characters and removes
 * new line characters from the input.
 * @param str: the input string
 * @param exceptions: array of characters to ignore while sanitizing
 */
int sanitized(char const *str, char const * exceptions);

#endif

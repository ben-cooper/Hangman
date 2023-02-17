#ifndef TST_H
#define TST_H

#include <stddef.h>

#define WILDCARD_CHR '-'
#define WILDCARD_STR "-"

/**
 * Structure for a single node in a ternary search tree
 */
struct tst_node {
	int is_word;
	char chr;
	struct tst_node *left;
	struct tst_node *right;
	struct tst_node *middle;
};

/**
 * Creates a new ternary search tree
 * @param str: the string to initialize the tree
 * @param idx: the current position in the string (used in recursion)
 * @param len: the length of the string
 */
struct tst_node *tst_create(char const *str, size_t idx, size_t len);

/**
 * Inserts a new string into the ternary search tree
 * @param root: pointer to the tree
 * @param str: the string to insert
 * @param idx: the current position in the string (used in recursion)
 */
void tst_insert(struct tst_node *root, char const *str, size_t idx, size_t len);

/**
 * Frees all memory used by the entire ternary search tree
 * @param root: pointer to the tst to destroy
 */
void tst_destroy(struct tst_node *root);

/**
 * Writes all matching words into the file descriptor
 * @param root: pointer to the tree
 * @param pattern: string pattern to search (ex. h_llo)
 * @param idx: the current position in the pattern string (used in recursion)
 * @param len: the length of the pattern string
 * @param wrong: array of letters that are blacklisted
 * @param fd: the file descriptor to write matching words into
 * @param buffer: string buffer to accumulate letters of words through recursion
 */
void tst_pattern_search(struct tst_node const *root, char const *pattern,
                        size_t idx, size_t len, char const *wrong, int fd,
                        char *buffer);

/**
 * Returns 1 if the word is present in the tree
 * @param root: pointer to the tree
 * @param str: the string to search for
 * @param idx: the current position of the string (used in recursion)
 * @param len: the length of the string
 */
int tst_search(struct tst_node const *root, char const *str, size_t idx,
               size_t len);

/**
 * Returns the height of the ternary search tree
 * @param root: pointer to the tree
 */
size_t tst_height(struct tst_node const *root);

#endif

#ifndef TST_H
#define TST_H

/**
 * Structure for a single node in a ternary search tree
 */
struct tst_node {
	char *word;
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
struct tst_node *tst_create(char *str, size_t idx, size_t len);

/**
 * Inserts a new string into the ternary search tree
 * @param tree: pointer to the tree
 * @param str: the string to insert
 * @param idx: the current position in the string (used in recursion)
 */
void tst_insert(struct tst_node *tree, char *str, size_t idx, size_t len);

/**
 * Frees all memory used by the entire ternary search tree
 * @param tree: pointer to the tst to destroy
 * @param destroy_str: flag, if set will free all strings in the tst nodes
 */
void tst_destroy(struct tst_node *tree, int destroy_str);

/**
 * Writes all matching words into the file descriptor
 * @param root: pointer to the tree
 * @param pattern: string pattern to search (ex. h_llo)
 * @param idx: the current position in the pattern string (used in recursion)
 * @param len: the length of the pattern string
 * @param wrong: array of letters that are blacklisted
 * @param fd: the file descriptor to write matching words into
 */
void tst_pattern_search(struct tst_node *root, char const *pattern, size_t idx,
                        size_t len, char const *wrong, int fd);

/**
 * Returns 1 if the word is present in the tree
 * @param tree: pointer to the tree
 * @param str: the string to search for
 * @param idx: the current position of the string (used in recursion)
 * @param len: the length of the string
 */
int tst_search(struct tst_node const *tree, char const *str, size_t idx,
               size_t len);

/**
 * Returns the height of the ternary search tree
 * @param tree: pointer to the tree
 */
size_t tst_height(struct tst_node const *tree);

#endif

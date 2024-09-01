#ifndef TST_H
#define TST_H

#include <stddef.h>
#include <sys/types.h>
#include "bool.h"

#define SEARCH_BUFFER_SIZE 100

#define TST_START_SIZE 1024
#define TST_SCALING 2

#define FILE_MAGIC "TSTCACHE"
#define FILE_MAGIC_SIZE sizeof(FILE_MAGIC) - 1

struct tst_cache_header {
	char magic[FILE_MAGIC_SIZE];
	unsigned trees : 16;
	unsigned char index_width;
	unsigned char node_size;
	bool little_endian;
};

/**
 * Structure for a single node in a ternary search tree
 */
struct tst_node {
	unsigned left;
	unsigned right;
	unsigned middle;
	bool is_word;
	char chr;
};

/**
 * Structure for a tree comprising many nodes
 */
struct tst_tree {
	size_t elements;
	size_t capacity;
	/* flexible array member */
	struct tst_node nodes[1];
};

/**
 * Initializes a new ternary search tree
 * @param capacity	The starting number of elements the tree can contain
 * @return a pointer to the new tree
 */
struct tst_tree *tst_init(size_t capacity);

/**
 * Inserts a new string into the ternary search tree
 * @param tree: pointer to the tree
 * @param str: the string to insert
 */
void tst_insert(struct tst_tree **tree_ptr, char const *str);

/**
 * Writes all matching words into the file descriptor
 * @param tree: pointer to the tree
 * @param pattern: string pattern to search (ex. h_llo)
 * @param wrong: array of letters that are blacklisted
 * @param fd: the file descriptor to write matching words into
 */
void tst_pattern_search(struct tst_tree const *tree, char const *pattern,
                        char const *wrong, char wildcard, int fd);

/**
 *	Resizes array of nodes to remove extra space.
 */
void tst_trim(struct tst_tree **tree);

/**
 * Serializes the tst tree to a file
 */
void tst_save_cache(struct tst_tree **trees, size_t len, int fd);

/**
 * Loads array of tst_tree's from a file
 */
struct tst_tree **tst_load_cache(int fd, size_t *num_trees);

#endif

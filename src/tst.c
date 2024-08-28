#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "tst.h"

struct tst_tree *tst_init(size_t capacity)
{
	struct tst_tree *tree =
	    e_malloc(sizeof(struct tst_tree) - sizeof(struct tst_node) +
	             sizeof(struct tst_node) * capacity);

	tree->elements = 0;
	tree->capacity = capacity;

	return tree;
}

void tst_insert(struct tst_tree **tree_ptr, char const *str)
{
	size_t n = 0;
	struct tst_tree *tree = *tree_ptr;
	struct tst_node *node = &tree->nodes[0];

	/* traversing through tree */
	while ((n != tree->elements) && (*str)) {
		node = &tree->nodes[n];

		if (*str < node->chr) {
			n = (node->left) ? (node->left) : (node->left =
			                                   tree->elements);
		} else if (*str > node->chr) {
			n = (node->right) ? (node->right) : (node->right =
			                                     tree->elements);
		} else {
			n = (node->middle) ? (node->middle) : (node->middle =
			                                       tree->elements);
			str++;
		}
	}

	/* reached bottom of the tree, insert the rest of the word */
	while (*str) {
		/* expanding tree if it's at capacity */
		if (tree->capacity == n) {
			*tree_ptr =
			    e_realloc(tree,
			              sizeof(struct tst_tree) - sizeof(struct tst_node) +
			              sizeof(struct tst_node) * tree->capacity *
			              TST_SCALING);

			/* refreshing pointer */
			tree = *tree_ptr;
			tree->capacity *= TST_SCALING;
		}
		/* building node */
		node = &tree->nodes[n];
		memset(node, 0, sizeof(struct tst_node));

		node->chr = *str;

		tree->elements++;
		str++;
		n++;

		if (*str)
			node->middle = n;
	}

	node->is_word = true;
}

static void tst_pattern_search_rec(struct tst_tree const *tree,
                                   char const *pattern, size_t n,
                                   char const *wrong, char wildcard, int fd,
                                   char *buffer, size_t i)
{
	struct tst_node const *node = &tree->nodes[n];

	if (n >= tree->elements)
		return;

	if ((node->left)
	    && ((pattern[i] == wildcard) || (pattern[i] < node->chr)))
		tst_pattern_search_rec(tree, pattern, node->left, wrong,
	                               wildcard, fd, buffer, i);

	if ((node->right)
	    && ((pattern[i] == wildcard) || (pattern[i] > node->chr)))
		tst_pattern_search_rec(tree, pattern, node->right, wrong,
	                               wildcard, fd, buffer, i);

	/* if node character is already present in pattern or wrong letters, wildcard cannot match it */
	if ((pattern[i] == wildcard) &&
	    (strchr(wrong, node->chr) || strchr(pattern, node->chr)))
		return;

	if ((pattern[i] == wildcard) || (pattern[i] == node->chr)) {
		buffer[i] = node->chr;

		/* if we are on the last character */
		if (pattern[i + 1] == '\0') {
			if (node->is_word) {
				buffer[i + 1] = '\0';
				e_write(fd, buffer, i + 1);
			}
		} else if (node->middle) {
			tst_pattern_search_rec(tree, pattern, node->middle,
			                       wrong, wildcard, fd, buffer,
			                       i + 1);
		}
	}
}

void tst_pattern_search(struct tst_tree const *tree, char const *pattern,
                        char const *wrong, char wildcard, int fd)
{
	char buffer[SEARCH_BUFFER_SIZE];
	tst_pattern_search_rec(tree, pattern, 0, wrong, wildcard, fd, buffer,
	                       0);
}

void tst_trim(struct tst_tree **tree)
{
	*tree =
	    realloc(*tree,
	            sizeof(struct tst_tree) - sizeof(struct tst_node) +
	            sizeof(struct tst_node) * (*tree)->elements);
	(*tree)->capacity = (*tree)->elements;
}

void tst_save_cache(struct tst_tree **trees, size_t len, int fd)
{
	size_t i;
	size_t size;

	struct tst_cache_header header;
	memcpy(header.magic, FILE_MAGIC, sizeof(header.magic));
	header.trees = len;

	e_write(fd, &header, sizeof(header));

	for (i = 0; i < len; i++) {
		size =
		    sizeof(struct tst_tree) - sizeof(struct tst_node) +
		    sizeof(struct tst_node) * trees[i]->elements;
		e_write(fd, trees[i], size);
	}
}

struct tst_tree **tst_load_cache(int fd, size_t *num_trees)
{
	size_t i;
	size_t len;
	char *file_buffer;
	struct tst_tree **trees;
	struct tst_cache_header header;

	/* getting file size */
	len = end_lseek(fd);
	start_lseek(fd);

	/* reading header */
	e_read(fd, &header, sizeof(header));
	*num_trees = header.trees;

	/* reading rest of file */
	file_buffer = e_malloc(len - sizeof(header));
	e_read(fd, file_buffer, len - sizeof(header));

	trees = e_malloc(sizeof(struct tst_tree *) * header.trees);

	for (i = 0; i < header.trees; i++) {
		trees[i] = (struct tst_tree *)file_buffer;

		/* finding next tree */
		file_buffer +=
		    sizeof(struct tst_tree) - sizeof(struct tst_node) +
		    sizeof(struct tst_node) * trees[i]->capacity;
	}

	return trees;
}

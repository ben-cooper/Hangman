#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "tst.h"

struct tst_tree *tst_init(size_t capacity)
{
	struct tst_tree *tree = e_malloc(sizeof(struct tst_tree) + sizeof(struct tst_node) * capacity);

	tree->elements = 0;
	tree->capacity = capacity;

	return tree;
}

void tst_insert(struct tst_tree **tree_ptr, char const *str, size_t node)
{
	struct tst_tree *tree = *tree_ptr;

	// if we are at the bottom of the tree, put the rest of the string in the tree
	if (tree->elements == node) {
		while (true) {

			// expanding tree if it's at capacity
			if (tree->capacity == node) {
				*tree_ptr = e_realloc(tree, sizeof(struct tst_tree) + sizeof(struct tst_node) * tree->capacity * TST_SCALING);
				tree = *tree_ptr;
				tree->capacity *= TST_SCALING;
			}

			memset(tree->array + node, 0, sizeof(struct tst_node));

			// building node
			tree->array[node].chr   = *str;

			tree->elements++;

			if (str[1] == '\0') {
				tree->array[node].is_word = true;
				break;
			}

			tree->array[node].middle  = node + 1;

			node++;
			str++;
		}
	} else {
		// navigate through the tree recursively until the end
		if (*str < tree->array[node].chr) {
			if (!tree->array[node].left)
					tree->array[node].left = tree->elements;

			tst_insert(tree_ptr, str, tree->array[node].left);
		} else if (*str > tree->array[node].chr) {
				if (!tree->array[node].right)
					tree->array[node].right = tree->elements;

			tst_insert(tree_ptr, str, tree->array[node].right);
		} else {
			// if we are on the last character
			if (str[1] == '\0') {
				tree->array[node].is_word = 1;
			} else {
				if (!tree->array[node].middle)
					tree->array[node].middle = tree->elements;

				tst_insert(tree_ptr, str + 1, tree->array[node].middle);
			}
		}
	}
}

void tst_pattern_search(struct tst_tree const *tree, char const *pattern,
                        size_t node, char const *wrong, char wildcard,
						int fd, char *buffer, size_t index)
{
	if (node >= tree->elements)
		return;

	if ((tree->array[node].left) && ((pattern[index] == wildcard) || (pattern[index] < tree->array[node].chr)))
		tst_pattern_search(tree, pattern, tree->array[node].left, wrong, wildcard, fd, buffer, index);

	if ((tree->array[node].right) && ((pattern[index] == wildcard) || (pattern[index] > tree->array[node].chr)))
		tst_pattern_search(tree, pattern, tree->array[node].right, wrong, wildcard, fd, buffer, index);

	// if node character is already present in pattern or wrong letters, wildcard cannot match it
	if ((pattern[index] == wildcard) &&
		(strchr(wrong, tree->array[node].chr) || strchr(pattern, tree->array[node].chr)))
		return;

	if ((pattern[index] == wildcard) || (pattern[index] == tree->array[node].chr)) {
		buffer[index] = tree->array[node].chr;

		// if we are on the last character
		if (pattern[index + 1] == '\0')  {
			if (tree->array[node].is_word) {
				buffer[index+1] = '\0';
				e_write(fd, buffer,  index + 1);
			}
		} else if (tree->array[node].middle) {
			tst_pattern_search(tree, pattern, tree->array[node].middle, wrong, wildcard, fd, buffer, index +  1);
		}
	}
}

void tst_trim(struct tst_tree **tree)
{
	*tree = realloc(*tree, sizeof(struct tst_tree) + sizeof(struct tst_node) * (*tree)->elements);
	(*tree)->capacity = (*tree)->elements;
}

void tst_save_cache(struct tst_tree **trees, size_t len, int fd)
{
	size_t i;
	size_t size;

	struct tst_cache_header header = {FILE_MAGIC, len};

	e_write(fd, &header, sizeof(header));

	for (i = 0; i < len; i++) {
		size = sizeof(struct tst_tree) + sizeof(struct tst_node) * trees[i]->elements;
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

	// getting file size
	len = end_lseek(fd);
	start_lseek(fd);

	// reading header
	e_read(fd, &header, sizeof(header));
	*num_trees = header.trees;

	// reading rest of file
	file_buffer = e_malloc(len - sizeof(header));
	e_read(fd, file_buffer, len - sizeof(header));

	trees = e_malloc(sizeof(struct tst_tree *) * header.trees);

	for (i = 0; i < header.trees; i++) {
		trees[i] = (struct tst_tree *) file_buffer;

		// finding next tree
		file_buffer += sizeof(struct tst_tree) + sizeof(struct tst_node) * trees[i]->capacity;
	}

	return trees;
}

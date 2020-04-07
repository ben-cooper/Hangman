#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "common.h"
#include "tst.h"

struct tst_node *tst_create(char *str, size_t idx, size_t len)
{
	struct tst_node *result = e_malloc(sizeof(struct tst_node));

	result->left = NULL;
	result->right = NULL;
	result->chr = str[idx];

	/* if on the last character */
	if ((len - idx) == 1) {
		result->middle = NULL;
		result->word = str;
	} else {
		/* recurse */
		result->middle = tst_create(str, idx + 1, len);
		result->word = NULL;
	}

	return result;
}

void tst_insert(struct tst_node *tree, char *str, size_t idx, size_t len)
{
	if (str[idx] < tree->chr) {
		/* left subtree */
		if (tree->left)
			tst_insert(tree->left, str, idx, len);
		else
			tree->left = tst_create(str, idx, len);

	} else if (str[idx] > tree->chr) {
		/* right subtree */
		if (tree->right)
			tst_insert(tree->right, str, idx, len);
		else
			tree->right = tst_create(str, idx, len);

	} else {
		/* base case last character */
		if ((len - idx) == 1) {
			tree->word = str;
		} else {
			/* middle subtree */
			if (tree->middle)
				tst_insert(tree->middle, str, idx + 1, len);
			else
				tree->middle = tst_create(str, idx + 1, len);
		}
	}
}

int tst_search(struct tst_node const *tree, char const *str, size_t idx,
               size_t len)
{
	if (!tree)
		return 0;

	if (str[idx] < tree->chr)
		return tree->left && tst_search(tree->left, str, idx, len);
	else if (str[idx] > tree->chr)
		return tree->right && tst_search(tree->right, str, idx, len);
	else
		return (((len - idx) == 1) && tree->word) ||
		       tst_search(tree->middle, str, idx + 1, len);
}

void tst_pattern_search(struct tst_node * root, char const *pattern,
                        size_t idx, size_t len, char const *wrong, int fd)
{
	if (!root)
		return;

	if ((pattern[idx] == '_') || (pattern[idx] < root->chr))
		tst_pattern_search(root->left, pattern, idx, len, wrong, fd);

	if ((pattern[idx] == '_') || (pattern[idx] > root->chr))
		tst_pattern_search(root->right, pattern, idx, len, wrong, fd);

	if ((pattern[idx] == '_') &&
		(index(wrong, root->chr) || index(pattern, root->chr)))
		return;

	if ((pattern[idx] == root->chr) || (pattern[idx] == '_')) {
		if ((len - idx) == 1) {
			if (root->word)
				e_write(fd, &(root->word), sizeof(root->word));
		} else {
			tst_pattern_search(root->middle, pattern, idx + 1, len, wrong, fd);
		}
	}
}

size_t tst_height(struct tst_node const *tree)
{
	size_t left_height;
	size_t right_height;
	size_t middle_height;
	size_t result;

	if (!tree)
		return 0;

	left_height = tst_height(tree->left) + 1;
	right_height = tst_height(tree->right) + 1;
	middle_height = tst_height(tree->middle) + 1;

	result = left_height > right_height ? left_height : right_height;
	result = result > middle_height ? result : middle_height;

	return result;
}

void tst_destroy(struct tst_node *tree, int destroy_str)
{
	if (!tree)
		return;

	tst_destroy(tree->left, destroy_str);
	tst_destroy(tree->right, destroy_str);
	tst_destroy(tree->middle, destroy_str);

	if (destroy_str)
		free(tree->word);

	free(tree);
}

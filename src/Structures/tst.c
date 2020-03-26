#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "common.h"
#include "tst.h"

tst_node *tst_create(char *str, size_t idx, size_t len)
{
	tst_node *result = (tst_node *) e_malloc(sizeof(tst_node));

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

void tst_insert(tst_node *tree, char *str, size_t idx, size_t len)
{
	if (str[idx] < tree->chr) {
		/* left subtree */
		if (tree->left) {
			tst_insert(tree->left, str, idx, len);
		} else {
			tree->left = tst_create(str, idx, len);
		}
	} else if (str[idx] > tree->chr) {
		/* right subtree */
		if (tree->right) {
			tst_insert(tree->right, str, idx, len);
		} else {
			tree->right = tst_create(str, idx, len);
		}
	} else {
		/* base case last character */
		if ((len - idx) == 1) {
			tree->word = str;
		} else {
			/* middle subtree */
			if (tree->middle) {
				tst_insert(tree->middle, str, idx + 1, len);
			} else {
				tree->middle = tst_create(str, idx + 1, len);
			}
		}
	}
}

/* return 0 if not in the tree, otherwise 1 */
int tst_search(tst_node const *tree, char const *str, size_t idx, size_t len)
{
	if (!tree) {
		return 0;
	}

	if (str[idx] < tree->chr) {
		/* search left */
		return tree->left && tst_search(tree->left, str, idx, len);
	} else if (str[idx] > tree->chr) {
		/* search right */
		return tree->right && tst_search(tree->right, str, idx, len);
	} else {
		return (((len - idx) == 1) && tree->word)
		    || tst_search(tree->middle, str, idx + 1, len);
	}
}

/* finds all words in the tst matching the string "pattern" while
excluding wrong letter (pattern ex: "b_n_n_") into given pipe */
void tst_pattern_search(tst_node * root, char const *pattern, size_t idx,
		   size_t len, char const *wrong, int fd)
{
	if (root) {

		/* recursing */
		if (pattern[idx] == '_') {
			tst_pattern_search(root->left, pattern, idx, len, wrong, fd);
			tst_pattern_search(root->right, pattern, idx, len, wrong, fd);

			/* if last character, check for match */
			if ((len - idx) == 1) {
				/* check if part of wrong letter */
				if ((!index(wrong, root->chr)) &&
				    (!index(pattern, root->chr)) &&
				    (root->word)) {
					/* match found */
					e_write(fd, &(root->word), sizeof(root->word));
				}
			} else {
				/* go to middle subtree */
				if ((!index(wrong, root->chr)) &&
				    (!index(pattern, root->chr))) {
					tst_pattern_search(root->middle, pattern,
						      idx + 1, len, wrong, fd);
				}
			}
		} else if (pattern[idx] < root->chr) {
			tst_pattern_search(root->left, pattern, idx, len, wrong, fd);
		} else if (pattern[idx] > root->chr) {
			tst_pattern_search(root->right, pattern, idx, len, wrong, fd);
		} else {

			/* character matches */
			if ((len - idx) == 1) {
				if (root->word) {
					/* macth found */
					e_write(fd, &(root->word), sizeof(root->word));
				}
			} else {
				/* middle subtree */
				tst_pattern_search(root->middle, pattern, idx + 1,
					      len, wrong, fd);
			}
		}
	}
}

size_t tst_height(tst_node const *tree)
{
	size_t left_height;
	size_t right_height;
	size_t middle_height;
	size_t result;

	if (!tree) {
		return 0;
	}

	left_height = tst_height(tree->left) + 1;
	right_height = tst_height(tree->right) + 1;
	middle_height = tst_height(tree->middle) + 1;

	result = left_height > right_height ? left_height : right_height;
	result = result > middle_height ? result : middle_height;

	return result;
}

/* free each string if destroy_str is 1 */
void tst_destroy(tst_node *tree, int destroy_str)
{
	if (tree) {
		tst_destroy(tree->left, destroy_str);
		tst_destroy(tree->right, destroy_str);
		tst_destroy(tree->middle, destroy_str);

		if (destroy_str) {
			free(tree->word);
		}

		free(tree);
	}
}

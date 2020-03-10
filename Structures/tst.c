#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "tst.h"

tst_node *tst_create(char *const str, size_t idx, size_t len)
{
	tst_node *result = e_malloc(sizeof(tst_node));

	result->left = NULL;
	result->right = NULL;
	result->chr = str[idx];

	// if on the last character
	if ((len - idx) == 1) {
		result->middle = NULL;
		result->word = str;
	} else {
		// recurse
		result->middle = tst_create(str, idx + 1, len);
		result->word = NULL;
	}

	return result;
}

void tst_insert(tst_node * tree, char *const str, size_t idx, size_t len)
{
	if (str[idx] < tree->chr) {
		// left subtree
		if (tree->left) {
			tst_insert(tree->left, str, idx, len);
		} else {
			tree->left = tst_create(str, idx, len);
		}
	} else if (str[idx] > tree->chr) {
		// right subtree
		if (tree->right) {
			tst_insert(tree->right, str, idx, len);
		} else {
			tree->right = tst_create(str, idx, len);
		}
	} else {
		// base case last character
		if ((len - idx) == 1) {
			tree->word = str;
		} else {
			// middle subtree
			if (tree->middle) {
				tst_insert(tree->middle, str, idx + 1, len);
			} else {
				tree->middle = tst_create(str, idx + 1, len);
			}
		}
	}
}

bool tst_search(tst_node const *tree, char *const str, size_t idx, size_t len)
{
	if (!tree) {
		return false;
	}

	if (str[idx] < tree->chr) {
		// search left
		return tree->left && tst_search(tree->left, str, idx, len);
	} else if (str[idx] > tree->chr) {
		// search right
		return tree->right && tst_search(tree->right, str, idx, len);
	} else {
		return (((len - idx) == 1) && tree->word)
		    || tst_search(tree->middle, str, idx + 1, len);
	}
}

void tst_destroy(tst_node * tree)
{
	if (tree) {
		tst_destroy(tree->left);
		tst_destroy(tree->right);
		tst_destroy(tree->middle);
		free(tree->word);
		free(tree);
	}
}

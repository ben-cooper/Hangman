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

void tst_insert(struct tst_node *root, char *str, size_t idx, size_t len)
{
	if (str[idx] < root->chr) {
		/* left subtree */
		if (root->left)
			tst_insert(root->left, str, idx, len);
		else
			root->left = tst_create(str, idx, len);

	} else if (str[idx] > root->chr) {
		/* right subtree */
		if (root->right)
			tst_insert(root->right, str, idx, len);
		else
			root->right = tst_create(str, idx, len);

	} else {
		/* base case last character */
		if ((len - idx) == 1) {
			root->word = str;
		} else {
			/* middle subtree */
			if (root->middle)
				tst_insert(root->middle, str, idx + 1, len);
			else
				root->middle = tst_create(str, idx + 1, len);
		}
	}
}

int tst_search(struct tst_node const *root, char const *str, size_t idx,
               size_t len)
{
	if (!root)
		return 0;

	if (str[idx] < root->chr)
		return root->left && tst_search(root->left, str, idx, len);
	else if (str[idx] > root->chr)
		return root->right && tst_search(root->right, str, idx, len);
	else
		return (((len - idx) == 1) && root->word) ||
		       tst_search(root->middle, str, idx + 1, len);
}

void tst_pattern_search(struct tst_node * root, char const *pattern,
                        size_t idx, size_t len, char const *wrong, int fd)
{
	if (!root)
		return;

	if ((pattern[idx] == WILDCARD_CHR) || (pattern[idx] < root->chr))
		tst_pattern_search(root->left, pattern, idx, len, wrong, fd);

	if ((pattern[idx] == WILDCARD_CHR) || (pattern[idx] > root->chr))
		tst_pattern_search(root->right, pattern, idx, len, wrong, fd);

	if ((pattern[idx] == WILDCARD_CHR) &&
		(strchr(wrong, root->chr) || strchr(pattern, root->chr)))
		return;

	if ((pattern[idx] == WILDCARD_CHR) || (pattern[idx] == root->chr)) {
		if ((len - idx) == 1) {
			if (root->word)
				e_write(fd, &(root->word), sizeof(root->word));
		} else {
			tst_pattern_search(root->middle, pattern, idx + 1, len, wrong, fd);
		}
	}
}

size_t tst_height(struct tst_node const *root)
{
	size_t left_height;
	size_t right_height;
	size_t middle_height;
	size_t result;

	if (!root)
		return 0;

	left_height = tst_height(root->left) + 1;
	right_height = tst_height(root->right) + 1;
	middle_height = tst_height(root->middle) + 1;

	result = left_height > right_height ? left_height : right_height;
	result = result > middle_height ? result : middle_height;

	return result;
}

void tst_destroy(struct tst_node *root, int destroy_str)
{
	if (!root)
		return;

	tst_destroy(root->left, destroy_str);
	tst_destroy(root->right, destroy_str);
	tst_destroy(root->middle, destroy_str);

	if (destroy_str)
		free(root->word);

	free(root);
}

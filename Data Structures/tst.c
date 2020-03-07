#include "common.h"
#include <string.h>
#include <stdbool.h>

typedef struct tst_node {
	char *word;
	char chr;
	struct tst_node *left;
	struct tst_node *right;
	struct tst_node *middle;
} tst_node;

tst_node *tst_create(char * const str, size_t idx, size_t len)
{
	tst_node *result = e_malloc(sizeof(tst_node));
	
	result->left = NULL;
	result->right = NULL;
	result->chr = str[idx];
	
	// if on the last character
	if ((len - idx) == 1)
	{
		result->middle = NULL;
		result->word = str;
	} else
	{
		// recurse
		result->middle = tst_create(str, idx + 1, len);
		result->word = NULL;
	}
	
	return result;
}

void tst_insert(tst_node *tree, char * const str, size_t idx, size_t len)
{
	if (str[idx] < tree->chr)
	{
		// left subtree
		if (tree->left)
		{
			tst_insert(tree->left, str, idx, len);
		} else
		{
			tree->left = tst_create(str, idx, len);
		}
	} else if (str[idx] > tree->chr)
	{
		// right subtree
		if (tree->right)
		{
			tst_insert(tree->right, str, idx, len);
		} else
		{
			tree->right = tst_create(str, idx, len);
		}
	} else
	{
		// base case last character
		if ((len - idx) == 1)
		{
			tree->word = str;
		} else
		{
			// middle subtree
			if (tree->middle)
			{
				tst_insert(tree->middle, str, idx+1, len);
			} else
			{
				tree->middle = tst_create(str, idx + 1, len);
			}
		}
	}
}

bool tst_search(tst_node const * tree, char * const str, size_t idx, size_t len)
{
	if (!tree)
	{
		return false;
	}
	
	if (str[idx] < tree->chr)
	{
		// search left
		return tree->left && tst_search(tree->left, str, idx, len);
	} else if (str[idx] > tree->chr)
	{
		// search right
		return tree->right && tst_search(tree->right, str, idx, len);
	} else
	{
		return ((len - idx) == 1) || tst_search(tree->middle, str, idx + 1, len);
	}
}

// testing
int main()
{
	char test1[] = "ben";
	char test2[] = "blade";
	char test3[] = "nick";
	char test4[] = "wyatt";
	char test5[] = "vlad";
	
	tst_node *root = tst_create(test1, 0, strlen(test1));
	tst_insert(root, test2, 0, strlen(test2));
	tst_insert(root, test3, 0, strlen(test3));
	tst_insert(root, test4, 0, strlen(test4));
	tst_insert(root, test5, 0, strlen(test5));
	
	printf("ben? %d\n", tst_search(root, "ben", 0, 3));
	printf("blade? %d\n", tst_search(root, "blade", 0, 5));
	printf("nick? %d\n", tst_search(root, "nick", 0, 4));
	printf("linux? %d\n", tst_search(root, "linux", 0, 5));
	
	return 0;
}

#ifndef TST_H
#define TST_H

typedef struct tst_node {
	char *word;
	char chr;
	struct tst_node *left;
	struct tst_node *right;
	struct tst_node *middle;
} tst_node;

tst_node *tst_create(char *str, size_t idx, size_t len);
void tst_insert(tst_node *tree, char *str, size_t idx, size_t len);
int tst_search(tst_node const *tree, char const *str, size_t idx, size_t len);
void tst_destroy(tst_node *tree, int destroy_str);
void tst_pattern_search(tst_node *root, char const *pattern, size_t idx,
		   size_t len, char const *wrong, int fd);
size_t tst_height(tst_node const *tree);

#endif

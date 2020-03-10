#ifndef TST_H
#define TST_H

typedef enum { false, true } bool;

typedef struct tst_node {
	char *word;
	char chr;
	struct tst_node *left;
	struct tst_node *right;
	struct tst_node *middle;
} tst_node;

tst_node *tst_create(char *const str, size_t idx, size_t len);
void tst_insert(tst_node * tree, char *const str, size_t idx, size_t len);
bool tst_search(tst_node const *tree, char *const str, size_t idx, size_t len);
void tst_destroy(tst_node * tree);

#endif

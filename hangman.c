#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "Misc/misc.h"
#include "Data Structures/common.h"
#include "Data Structures/d_array.h"
#include "Data Structures/tst.h"

#define MAX_STRING_SIZE 100
#define ARRAY_START_SIZE 100

/* finds all word in the tst matching the strring "hangman" while
excluding wrong letter (hangman ex: "b_n_n_" */
void hangman_words(tst_node *root, char const *hangman, size_t idx,
			size_t len, char const *wrong, d_array * output)
{
	if (root) {
		
		// recursing
		if (hangman[idx] == '_') {
			hangman_words(root->left, hangman, idx, len, wrong, output);
			hangman_words(root->right, hangman, idx, len, wrong, output);
			
			// if last character, check for match
			if ((len - idx) == 1) {
				// check if part of wrong letter
				if ((!index(wrong, root->chr)) && (root->word)) {
					// match found
					d_array_insert(output, root->word);
				}
			
			} else {
				// go to middle subtree
				if (!index(wrong, root->chr)) {
					hangman_words(root->middle, hangman, idx + 1, len, wrong, output);
				}
			}
		} else if (hangman[idx] < root->chr) {
			hangman_words(root->left, hangman, idx, len, wrong, output);
		} else if (hangman[idx] > root->chr) {
			hangman_words(root->right, hangman, idx, len, wrong, output);
		} else {
			// character matches
			
			if ((len - idx) == 1) {
				if (root->word) {
					// macth found
					d_array_insert(output, root->word);
				}
			} else {
				// middle subtree
				hangman_words(root->middle, hangman, idx + 1, len, wrong, output);
			}
		}
	}
}


/* returns array of ternary search trees based on the number of
worker threads */
tst_node **initialize_words(FILE * word_list, size_t workers)
{
	size_t len;
	char *str;
	char str_buffer[MAX_STRING_SIZE];
	d_array *array = d_array_create(ARRAY_START_SIZE);
	tst_node **roots;
	size_t idx;

	// reading file
	while (fgets(str_buffer, MAX_STRING_SIZE, word_list)) {
		// coping and sanitizing
		len = strlen(str_buffer);
		str = e_malloc((len + 1) * sizeof(char));
		strcpy(str, str_buffer);
		sanitize(str, len);

		// adding to array
		d_array_insert(array, str);
	}

	// shuffling to avoid degenerate trees
	shuffle(array->array, array->elements);

	// initializing ternary search trees
	roots = e_malloc(workers * sizeof(tst_node *));

	for (idx = 0; idx < workers; idx++) {
		str = array->array[idx];
		len = strlen(str);
		roots[idx] = tst_create(str, 0, len);
	}

	// seperating array into separate ternary search trees
	// starting after the first words used to initialize the tst's
	for (idx = workers; idx < array->elements; idx++) {
		str = array->array[idx];
		len = strlen(str);
		tst_insert(roots[idx % workers], str, 0, len);
	}

	// freeing array memory
	d_array_destroy(array);

	return roots;
}

int main(int argc, char **argv)
{
	FILE *word_list;
	size_t workers;
	size_t idx;
	int error;
	tst_node **roots;

	// checking arguments
	if (argc == 2) {
		// no word list given, using unix
		if (!(word_list = fopen("/usr/share/dict/words", "r"))) {
			perror("fopen");
			fprintf(stderr, "Unix word list not found.  Please "
				"give path to word list as argument.\n");
			exit(EXIT_FAILURE);
		}
	} else if (argc == 3) {
		// using word list specified
		if (!(word_list = fopen(argv[2], "r"))) {
			perror("fopen");
			exit(EXIT_FAILURE);
		}
	} else {
		fprintf(stderr,
			"usage: %s workers [path_to_word_list]\n",
			argv[0]);
		exit(EXIT_FAILURE);
	}

	// getting number of worker threads
	if (!(error = sscanf(argv[1], "%zu", &workers))) {
		perror("scanf");
		exit(EXIT_FAILURE);
	}
	// initialization
	roots = initialize_words(word_list, workers);
	fclose(word_list);

	// user input loop
		// quick test
		d_array *test_output = d_array_create(100);
		hangman_words(roots[0], "b_n_n_", 0, 6, "fde", test_output);

	// freeing memory
	for (idx = 0; idx < workers; idx++) {
		tst_destroy(roots[idx]);
	}
	free(roots);
}

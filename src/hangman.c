#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <strings.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "Misc/misc.h"
#include "Structures/common.h"
#include "Structures/d_array.h"
#include "Structures/tst.h"

#define MAX_STRING_SIZE 100
#define ARRAY_START_SIZE 100

/* finds all word in the tst matching the strring "hangman" while
excluding wrong letter (hangman ex: "b_n_n_") into given pipe */
void hangman_words(tst_node * root, char const *hangman, size_t idx,
		   size_t len, char const *wrong, int fd)
{
	if (root) {

		/* recursing */
		if (hangman[idx] == '_') {
			hangman_words(root->left, hangman, idx, len, wrong, fd);
			hangman_words(root->right, hangman, idx, len, wrong, fd);

			/* if last character, check for match */
			if ((len - idx) == 1) {
				/* check if part of wrong letter */
				if ((!index(wrong, root->chr)) &&
				    (!index(hangman, root->chr)) &&
				    (root->word)) {
					/* match found */
					write(fd, &(root->word),
					      sizeof(root->word));
				}

			} else {
				/* go to middle subtree */
				if (!index(wrong, root->chr)) {
					hangman_words(root->middle, hangman,
						      idx + 1, len, wrong, fd);
				}
			}
		} else if (hangman[idx] < root->chr) {
			hangman_words(root->left, hangman, idx, len, wrong, fd);
		} else if (hangman[idx] > root->chr) {
			hangman_words(root->right, hangman, idx, len, wrong, fd);
		} else {
			/* character matches */

			if ((len - idx) == 1) {
				if (root->word) {
					/* macth found */
					write(fd, &(root->word),
					      sizeof(root->word));
				}
			} else {
				/* middle subtree */
				hangman_words(root->middle, hangman, idx + 1, len, wrong, fd);
			}
		}
	}
}

void fork_search(tst_node ** roots, char const *hangman, size_t len,
		 char const *wrong, size_t workers)
{
	int fd[2];
	size_t i;
	pid_t child;
	char *word = NULL;
	d_array *found_words = d_array_create(ARRAY_START_SIZE);

	if (pipe(fd) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	/* creating children */
	for (i = 0; i < workers; i++) {
		switch (child = fork()) {

			/* failed */
		case -1:
			perror("fork");
			exit(EXIT_FAILURE);
			break;

			/* child */
		case 0:
			close(fd[0]);
			hangman_words(roots[i], hangman, 0, len, wrong, fd[1]);
			/* sending NULL to signal exit */
			write(fd[1], &word, sizeof(word));
			close(fd[1]);
			exit(EXIT_SUCCESS);
			break;

			/* parent */
		default:
			break;
		}
	}

	/* parent reads until all children have sent NULL */
	i = 0;
	while (i < workers) {
		read(fd[0], &word, sizeof(word));
		if (!word) {
			i++;
		} else {
			d_array_insert(found_words, word);
		}
	}

	printf("\nPossible words:\n");
	d_array_print(found_words);
	printf("\n\nLetter Probabilities:\n");
	print_probabilities(found_words->array, found_words->elements, hangman);

	/* freeing memory */
	d_array_destroy(found_words);
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
	int clean;

	/* reading file */
	while (fgets(str_buffer, MAX_STRING_SIZE, word_list)) {
		/* copying and sanitizing */
		str = (char *) e_malloc(MAX_STRING_SIZE * sizeof(char));
		strcpy(str, str_buffer);

		if ((clean = sanitize(str, ""))) {
			/* adding to array */
			d_array_insert(array, str);
		} else {
			free(str);
		}
	}
	
	/* checking if there are enough words for the number of threads */
	if (array->elements < workers) {
		fprintf(stderr, "Not enough words for number of threads.\n");
		exit(EXIT_FAILURE);
	}

	/* shuffling to avoid degenerate trees */
	shuffle(array->array, array->elements);

	/* initializing ternary search trees */
	roots = (tst_node **) e_malloc(workers * sizeof(tst_node *));

	for (idx = 0; idx < workers; idx++) {
		str = array->array[idx];
		len = strlen(str);
		roots[idx] = tst_create(str, 0, len);
	}

	/* seperating array into separate ternary search trees
	   starting after the first words used to initialize the tst's */
	for (idx = workers; idx < array->elements; idx++) {
		str = array->array[idx];
		len = strlen(str);
		tst_insert(roots[idx % workers], str, 0, len);
	}

	/* freeing array memory */
	d_array_destroy(array);

	return roots;
}

int main(int argc, char **argv)
{
	FILE *word_list;
	size_t workers;
	size_t idx;
	int bytes;
	tst_node **roots;
	char *hangman;
	char *wrong;

	/* checking arguments */
	if (argc == 2) {
		/* no word list given, using unix */
		if (!(word_list = fopen("/usr/share/dict/words", "r"))) {
			perror("fopen");
			fprintf(stderr, "Unix word list not found.  Please "
				"give path to word list as argument.\n");
			return EXIT_FAILURE;
		}
	} else if (argc == 3) {
		/* using word list specified */
		if (!(word_list = fopen(argv[2], "r"))) {
			perror("fopen");
			return EXIT_FAILURE;
		}
	} else {
		fprintf(stderr,
			"usage: %s workers [path_to_word_list]\n", argv[0]);
		return EXIT_FAILURE;
	}

	/* getting number of worker threads */
	if (!(bytes = sscanf(argv[1], "%zu", &workers)) || !workers) {
		fprintf(stderr, "Invalid number of worker threads.\n");
		return EXIT_FAILURE;
	}
	/* initialization */
	roots = initialize_words(word_list, workers);
	fclose(word_list);

	/* user input loop */
	while (1) {

		hangman = readline("Hangman string: ");
		if (!hangman)
			break;
		wrong = readline("Wrong letters: ");
		if (!wrong)
			break;

		if ((sanitize(hangman, "_")) && (sanitize(wrong, ""))) {
			/* input is clean */
			fork_search(roots, hangman, strlen(hangman), wrong, workers);
		} else {
			fprintf(stderr, "Invalid input!\n");
		}

		add_history(hangman);
		add_history(wrong);
		free(hangman);
		free(wrong);
		printf("\n\n");
	}

	printf("\n");

	/* freeing memory */
	for (idx = 0; idx < workers; idx++) {
		tst_destroy(roots[idx]);
	}
	free(roots);
	rl_clear_history();

	return EXIT_SUCCESS;
}

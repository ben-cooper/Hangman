#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "Misc/misc.h"
#include "Structures/common.h"
#include "Structures/d_array.h"
#include "Structures/tst.h"

#define MAX_STRING_SIZE 100
#define ARRAY_START_SIZE 100

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
			tst_pattern_search(roots[i], hangman, 0, len, wrong, fd[1]);
			/* sending NULL to signal exit */
			e_write(fd[1], &word, sizeof(word));
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
		e_read(fd[0], &word, sizeof(word));
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
	d_array_destroy(found_words, 0);
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

	/* separating array into separate ternary search trees
	   starting after the first words used to initialize the tst's */
	for (idx = workers; idx < array->elements; idx++) {
		str = array->array[idx];
		len = strlen(str);
		tst_insert(roots[idx % workers], str, 0, len);
	}

	/* freeing array memory */
	d_array_destroy(array, 0);

	return roots;
}

int main(int argc, char **argv)
{
	FILE *word_list;
	size_t idx;
	int bytes;
	int option;
	tst_node **roots;
	char *hangman;
	char *wrong;

	/* default arguments */
	size_t workers = 1;
	char *dict_path = "/usr/share/dict/words";

	/* getting arguemtns */
	while ((option = getopt(argc, argv, "t:f:")) != -1) {

		switch (option) {
		case 't':
			/* getting number of worker threads */
			if (!(bytes = sscanf(optarg, "%zu", &workers))
			    || !workers) {
				fprintf(stderr,
					"Invalid number of worker threads: %s\n", optarg);
				return EXIT_FAILURE;
			}
			break;

		case 'f':
			dict_path = optarg;
			break;

		default:
			fprintf(stderr, "usage: %s [-t worker_threads] "
				"[-f path_to_word_list]\n", argv[0]);
			exit(EXIT_FAILURE);
			break;
		}
	}

	/* using word list specified */
	if (!(word_list = fopen(dict_path, "r"))) {
		perror("fopen");
		return EXIT_FAILURE;
	}

	/* initialization */
	roots = initialize_words(word_list, workers);
	fclose(word_list);

	/* user input loop */
	while ((hangman = readline("Hangman string: ")) &&
	       (wrong = readline("Wrong letters: "))) {

		if (sanitize(hangman, "_")) {
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
		tst_destroy(roots[idx], 1);
	}
	free(roots);
	rl_clear_history();

	return EXIT_SUCCESS;
}

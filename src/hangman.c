#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <string.h>
#include <histedit.h>
#include "Misc/misc.h"
#include "Structures/common.h"
#include "Structures/d_array.h"
#include "Structures/tst.h"

#define MAX_STRING_SIZE 100
#define ARRAY_START_SIZE 100

#define USAGE "usage: %s [-t worker_threads] [-f path_to_word_list]\n"
#define INVALID_WORKERS "Invalid number of worker threads: %s\n"

/**
 * Creates child processes to search through an array of ternary search trees
 * for a given pattern.
 * @param roots: array of tst to search (one tst per child)
 * @param hangman: the pattern string (ex. h_llo)
 * @param len: the length of the string
 * @param wrong: array of letters to exclude from the pattern
 * @param workers: the number of child processes to create
 */
void fork_search(struct tst_node ** roots, char const *hangman, size_t len,
                 char const *wrong, unsigned workers)
{
	int fd[2];
	unsigned i;
	char *word = NULL;
	struct d_array *found_words = d_array_create(ARRAY_START_SIZE);

	if (pipe(fd) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < workers; i++) {
		switch (fork()) {

		/* failed */
		case -1:
			perror("fork");
			exit(EXIT_FAILURE);

		/* child */
		case 0:
			close(fd[0]);
			tst_pattern_search(roots[i], hangman, 0, len, wrong, fd[1]);
			/* sending NULL to signal exit */
			e_write(fd[1], &word, sizeof(word));
			close(fd[1]);
			exit(EXIT_SUCCESS);

		/* parent */
		default:
			break;
		}
	}

	/* parent reads until all children have sent NULL */
	i = 0;
	while (i < workers) {
		e_read(fd[0], &word, sizeof(word));

		if (!word)
			i++;
		else
			d_array_insert(found_words, word);
	}

	if (found_words->elements == 0) {
		printf("\nCould not find any possible words!");
	} else {
		printf("\nPossible words:\n");
		d_array_print(found_words);
		printf("\nWords found: %lu\n", (unsigned long) found_words->elements);
		printf("\nLetter probabilities:\n");
		print_probability(found_words->array, found_words->elements, hangman);
	}

	d_array_destroy(found_words, 0);
}

/**
 * Returns an array of ternary search trees based on the number of child
 * processes to be created
 * @param word_list: file descriptor to read
 * @param workers: the number of child processes to be created
 */
struct tst_node **initialize_words(FILE * word_list, unsigned workers)
{
	size_t len;
	size_t i;
	char *str;
	char str_buffer[MAX_STRING_SIZE];
	struct d_array *array = d_array_create(ARRAY_START_SIZE);
	struct tst_node **roots;

	while (fgets(str_buffer, MAX_STRING_SIZE, word_list)) {

		str = e_malloc(MAX_STRING_SIZE * sizeof(char));
		strcpy(str, str_buffer);

		/* removing newline */
		str[strcspn(str, "\n")] = '\0';

		if (sanitized(str, ""))
			d_array_insert(array, str);
		else
			free(str);
	}

	if (array->elements < workers) {
		fprintf(stderr, "Not enough words for number of threads.\n");
		exit(EXIT_FAILURE);
	}

	shuffle(array->array, array->elements);

	/* initializing ternary search trees */
	roots = e_malloc(workers * sizeof(struct tst_node *));

	for (i = 0; i < workers; i++) {
		str = array->array[i];
		len = strlen(str);
		roots[i] = tst_create(str, 0, len);
	}

	/* separating array into separate ternary search trees
	   starting after the first words used to initialize the tst's */
	for (i = workers; i < array->elements; i++) {
		str = array->array[i];
		len = strlen(str);
		tst_insert(roots[i % workers], str, 0, len);
	}

	d_array_destroy(array, 0);

	return roots;
}

char * hangman_prompt(EditLine *e) {
	(void) e;
	return "Hangman string: ";
}

char * wrong_prompt(EditLine *e) {
	(void) e;
	return "Wrong letters: ";
}

int main(int argc, char **argv)
{
	FILE *word_list;
	unsigned i;
	int count;
	int option;
	struct tst_node **roots;
	char hangman[MAX_STRING_SIZE];
	char wrong[MAX_STRING_SIZE];
	char const *hangman_buffer;
	char const *wrong_buffer;
	EditLine *hangman_line;
	EditLine *wrong_line;

	/* default arguments */
	unsigned workers = 1;
	char const *dict_path = "/usr/share/dict/words";

	/* getting arguments */
	while ((option = getopt(argc, argv, "t:f:h")) != -1) {

		switch (option) {

		case 't':
			/* getting number of worker threads */
			if ((count = atoi(optarg)) < 1) {
				fprintf(stderr, INVALID_WORKERS, optarg);
				return EXIT_FAILURE;
			}
			workers = count;
			break;
		case 'f':
			dict_path = optarg;
			break;

		default:
			fprintf(stderr, USAGE, argv[0]);
			return EXIT_FAILURE;
		}
	}

	if (!(word_list = fopen(dict_path, "r"))) {
		perror("fopen");
		return EXIT_FAILURE;
	}

	roots = initialize_words(word_list, workers);
	fclose(word_list);

	/* initializing editlines */
	hangman_line = el_init(argv[0], stdin, stdout, stderr);
	el_set(hangman_line, EL_PROMPT, &hangman_prompt);
	el_set(hangman_line, EL_EDITOR, "emacs");
	wrong_line = el_init(argv[0], stdin, stdout, stderr);
	el_set(wrong_line, EL_PROMPT, &wrong_prompt);
	el_set(wrong_line, EL_EDITOR, "emacs");

	/* user input loop */
	while ((hangman_buffer = el_gets(hangman_line, &count)) &&
	       (wrong_buffer = el_gets(wrong_line, &count))) {

		strcpy(hangman, hangman_buffer);
		strcpy(wrong, wrong_buffer);

		/* removing newline */
		hangman[strcspn(hangman, "\n")] = '\0';
		wrong[strcspn(wrong, "\n")] = '\0';

		el_push(hangman_line, hangman);
		el_push(wrong_line, wrong);

		if (sanitized(hangman, WILDCARD_STR))
			fork_search(roots, hangman, strlen(hangman), wrong, workers);
		else
			fprintf(stderr, "Invalid input!\n");

		printf("\n\n");
	}

	printf("\n");

	/* freeing memory */
	for (i = 0; i < workers; i++)
		tst_destroy(roots[i], 1);

	free(roots);
	el_end(hangman_line);
	el_end(wrong_line);

	return EXIT_SUCCESS;
}

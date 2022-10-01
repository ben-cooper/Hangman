#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/types.h>
#include <string.h>
#include <histedit.h>
#include "word_utilities.h"
#include "common.h"
#include "tst.h"

#define MAX_STRING_SIZE 100

#define DEFAULT_WORKERS 1
#define DEFAULT_DICTIONARY "/usr/share/dict/words"

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
                 char const *wrong, unsigned long workers)
{
	int fd[2];
	unsigned long i;
	char word[MAX_STRING_SIZE];
	char tst_buffer[MAX_STRING_SIZE];

	e_pipe(fd);

	for (i = 0; i < workers; i++) {
		switch (fork()) {

		/* failed */
		case -1:
			perror("fork");
			exit(EXIT_FAILURE);

		/* child */
		case 0:
			e_close(fd[0]);
			tst_pattern_search(roots[i], hangman, 0, len, wrong, fd[1], tst_buffer);
			e_close(fd[1]);
			exit(EXIT_SUCCESS);

		/* parent */
		default:
			break;
		}
	}

	e_close(fd[1]);

	printf("\nPossible word(s):\n");

	while (read(fd[0], word, len) > 0) {
		word[len] = '\0';
		process_word(word, len);
		print_next_word(word, len);
	}

	reset_word_position();

	e_close(fd[0]);

	printf("\n\n");
	print_probability(hangman);
}

/**
 * Returns an array of ternary search trees based on the number of child
 * processes to be created
 * @param word_list: file descriptor to read
 * @param workers: the number of child processes to be created
 */
struct tst_node **initialize_words(int word_list, unsigned long workers)
{
	size_t len;
	size_t i;
	size_t words = 0;
	char *str;
	char *file_buffer;
	char **array;
	struct tst_node **roots;

	/* getting length of file */
	len = end_lseek(word_list);
	start_lseek(word_list);

	/* reading file */
	file_buffer = e_malloc(len+1);
	e_read(word_list, file_buffer, len);
	file_buffer[len] = '\0';

	/* counting newlines */
	for (i = 0; i < len; i++)
		if (file_buffer[i] == '\n')
			words++;

	/* check if file ends with newline */
	if (file_buffer[i - 1] != '\n')
		words++;

	/* tokenizing */
	array = e_malloc(words * sizeof(char *));
	i = 0;

	str = strtok(file_buffer, "\n");
	while (str)
	{
		if (sanitized(str, "")) {
			array[i] = str;
			i++;
		}

		str = strtok(NULL, "\n");
	}

	if (i != words)
		printf("%lu word(s) not added.\n", words - i);

	words = i;

	if (words < workers) {
		fprintf(stderr, "Not enough words for number of threads.\n");
		exit(EXIT_FAILURE);
	}

	shuffle(array, words);

	/* initializing ternary search trees */
	roots = e_malloc(workers * sizeof(struct tst_node *));

	for (i = 0; i < workers; i++) {
		len = strlen(array[i]);
		roots[i] = tst_create(array[i], 0, len);
	}

	/* separating array into separate ternary search trees
	   starting after the first words used to initialize the tst's */
	for (i = workers; i < words; i++) {
		len = strlen(array[i]);
		tst_insert(roots[i % workers], array[i], 0, len);
	}

	free(file_buffer);
	free(array);

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
	long input;
	int option;
	int word_list;
	struct tst_node **roots;
	char *endptr;
	char hangman[MAX_STRING_SIZE];
	char wrong[MAX_STRING_SIZE];
	char const *hangman_buffer;
	char const *wrong_buffer;
	unsigned long i;
	EditLine *hangman_line;
	EditLine *wrong_line;

	/* default arguments */
	unsigned long workers = DEFAULT_WORKERS;
	char const *dict_path = DEFAULT_DICTIONARY;

	/* getting arguments */
	while ((option = getopt(argc, argv, "t:f:h")) != -1) {

		switch (option) {

		case 't':
			/* getting number of worker threads */
			input = strtol(optarg, &endptr, 10);
			if ((input < 1) || (*endptr != '\0')) {
				fprintf(stderr, INVALID_WORKERS, optarg);
				return EXIT_FAILURE;
			}

			workers = input;
			break;
		case 'f':
			dict_path = optarg;
			break;

		default:
			fprintf(stderr, USAGE, argv[0]);
			return EXIT_FAILURE;
		}
	}

	/* reading dictionary */
	word_list = e_open(dict_path, O_RDONLY);
	roots = initialize_words(word_list, workers);
	e_close(word_list);

	/* initializing editlines */
	hangman_line = el_init(argv[0], stdin, stdout, stderr);
	el_set(hangman_line, EL_PROMPT, &hangman_prompt);
	el_set(hangman_line, EL_EDITOR, "emacs");
	wrong_line = el_init(argv[0], stdin, stdout, stderr);
	el_set(wrong_line, EL_PROMPT, &wrong_prompt);
	el_set(wrong_line, EL_EDITOR, "emacs");

	/* user input loop */
	while ((hangman_buffer = el_gets(hangman_line, &option)) &&
	       (wrong_buffer = el_gets(wrong_line, &option))) {

		strcpy(hangman, hangman_buffer);
		strcpy(wrong, wrong_buffer);

		/* removing newline */
		hangman[strcspn(hangman, "\n")] = '\0';
		wrong[strcspn(wrong, "\n")] = '\0';

		el_push(hangman_line, hangman);
		el_push(wrong_line, wrong);

		if (sanitized(hangman, WILDCARD_STR) && sanitized(wrong, ""))
			fork_search(roots, hangman, strlen(hangman), wrong, workers);
		else
			fprintf(stderr, "Invalid input!\n");

		printf("\n\n");
	}

	printf("\n");

	/* freeing memory */
	for (i = 0; i < workers; i++)
		tst_destroy(roots[i]);

	free(roots);
	el_end(hangman_line);
	el_end(wrong_line);

	return EXIT_SUCCESS;
}

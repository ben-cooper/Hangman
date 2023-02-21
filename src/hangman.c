#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <getopt.h>
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
 * @param pattern: the pattern string (ex. h_llo)
 * @param exclude: array of letters to exclude from the pattern
 * @param workers: the number of child processes to create
 */
void fork_search(struct tst_node ** roots, char const *pattern,
                 char const *exclude, size_t workers)
{
	int fd[2];
	char word[MAX_STRING_SIZE];
	char tst_buffer[MAX_STRING_SIZE];
	size_t worker;
	size_t len = strlen(pattern);

	e_pipe(fd);

	for (worker = 0; worker < workers; worker++) {
		switch (fork()) {

		/* failed */
		case -1:
			perror("fork");
			exit(EXIT_FAILURE);

		/* child */
		case 0:
			e_close(fd[0]);
			tst_pattern_search(roots[worker], pattern, 0, len, exclude, fd[1], tst_buffer);
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

	e_close(fd[0]);

	printf("\n\n");
	print_probability(pattern);

	reset_words();
}

/**
 * Returns an array of ternary search trees based on the number of child
 * processes to be created
 * @param word_list: file descriptor to read
 * @param workers: the number of child processes to be created
 */
struct tst_node **initialize_words(int word_list, size_t workers)
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
/**
  * Gets text from stdin and stores it in buffer
  * Return 1 if input was valid
  * @param prompt: The text to display before reading from stdin
  * @param output: The array to output the text into
  * @param size: The maximum size of the buffer
  * @param display_last: If equals 1, show the last string entered into the buffer
  */
int get_user_input(char *prompt, char *output, size_t size, int display_last)
{
	char buffer[MAX_STRING_SIZE];

	/* loop until valid input or EOF received */
	while (1)
	{
		if (display_last)
			printf("Last input:     %s\n", output);

		printf("%s", prompt);
		fflush(stdout);

		if (fgets(buffer, size, stdin) == NULL)
			return 0;

		/* removing newline */
		buffer[strcspn(buffer, "\n")] = '\0';

		if (sanitized(buffer, WILDCARD_STR))
		{
			strncpy(output, buffer, size);
			return 1;
		}

		fprintf(stderr, "Invalid input!\n\n");
	}
}


int main(int argc, char **argv)
{
	long input;
	int option;
	int word_list;
	size_t worker;
	struct tst_node **roots;
	char *endptr;
	char hangman[MAX_STRING_SIZE];
	char wrong[MAX_STRING_SIZE];

	/* default arguments */
	size_t workers = DEFAULT_WORKERS;
	char const *dict_path = DEFAULT_DICTIONARY;

	/* getting arguments */
	while ((option = getopt(argc, argv, "t:f:h")) != -1) {

		switch (option) {

		case 't':
			/* getting number of worker threads */
			input = strtol(optarg, &endptr, 10);
			if ((input < 1) || (*endptr != '\0') || (input > LONG_MAX)) {
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

	/* keeps track if user has entered input before */
	input = 0;

	/* user input loop */
	while(1) {

		if (get_user_input("Hangman string: ", hangman, MAX_STRING_SIZE, input) == 0)
			break;

		if (input == 1)
			printf("\n");

		if (get_user_input("Wrong letters:  ", wrong, MAX_STRING_SIZE, input) == 0)
			break;

		fork_search(roots, hangman, wrong, workers);

		printf("\n\n");
		input = 1;

	}

	printf("\n");

	/* freeing memory */
	for (worker = 0; worker < workers; worker++)
		tst_destroy(roots[worker]);

	free(roots);

	return EXIT_SUCCESS;
}

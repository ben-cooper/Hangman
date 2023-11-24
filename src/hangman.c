#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <getopt.h>
#include <sys/stat.h>
#include "word_utilities.h"
#include "common.h"
#include "tst.h"

#define MAX_STRING_SIZE 100

#define DEFAULT_WORKERS 1
#define DEFAULT_DICTIONARY "/usr/share/dict/words"

#define CACHE ".word_cache"
#define CACHE_PERM 0644

#define USAGE "usage: %s [-t worker_threads] [-f path_to_word_list] [-c]\n"
#define INVALID_WORKERS "Invalid number of worker threads: %s\n"

/**
 * Creates child processes to search through an array of ternary search trees
 * for a given pattern.
 * @param trees: array of tst to search (one tst per child)
 * @param pattern: the pattern string (ex. h-llo)
 * @param exclude: array of letters to exclude from the pattern
 * @param threads: the number of child processes to create
 */
void fork_search(struct tst_tree **trees, char const *pattern,
                 char const *exclude, size_t threads)
{
	int fd[2];
	char word[MAX_STRING_SIZE];
	size_t thread;
	size_t len = strlen(pattern);

	e_pipe(fd);

	for (thread = 0; thread < threads; thread++) {
		switch (fork()) {

		// failed
		case -1:
			perror("fork");
			exit(EXIT_FAILURE);

		// child
		case 0:
			e_close(fd[0]);
			tst_pattern_search(trees[thread], pattern, exclude,
			                   WILDCARD, fd[1]);
			e_close(fd[1]);
			exit(EXIT_SUCCESS);

		// parent
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
 * @param word_list: string containing all words seperated by newlines
 * @param threads: the number of child processes to be created
 */
struct tst_tree **initialize_words(char *dict, size_t threads)
{
	size_t i;
	size_t words = 0;
	char *str;
	char **array;
	struct tst_tree **trees;

	// counting newlines
	for (i = 0; dict[i] != '\0'; i++)
		if (dict[i] == '\n')
			words++;

	// check if file ends with newline
	if (dict[i - 1] != '\n')
		words++;

	// tokenizing
	array = e_malloc(words * sizeof(char *));
	i = 0;

	str = strtok(dict, "\n");
	while (str) {
		if (sanitized(str)) {
			array[i] = str;
			i++;
		}

		str = strtok(NULL, "\n");
	}

	if (i != words)
		printf("%zu word(s) not added.\n", words - i);

	words = i;

	if (words < threads) {
		fprintf(stderr, "Not enough words for number of threads.\n");
		exit(EXIT_FAILURE);
	}
	shuffle(array, words);

	// initializing ternary search trees
	trees = e_malloc(threads * sizeof(struct tst_tree *));

	for (i = 0; i < threads; i++)
		trees[i] = tst_init(TST_START_SIZE);

	// separating array into separate ternary search trees
	for (i = 0; i < words; i++)
		tst_insert(trees + (i % threads), array[i]);

	free(array);

	for (i = 0; i < threads; i++)
		tst_trim(trees + i);

	return trees;
}

/**
  * Gets text from stdin and stores it in buffer
  * Return true if input was valid
  * @param prompt: The text to display before reading from stdin
  * @param output: The array to output the text into
  * @param display_last: If equals 1, show the last string entered into the buffer
  */
bool get_user_input(char *prompt, char *output, bool display_last)
{
	char buffer[MAX_STRING_SIZE];

	// loop until valid input or EOF received
	while (true) {
		if (display_last)
			printf("Last input:     %s\n", output);

		printf("%s", prompt);
		fflush(stdout);

		if (fgets(buffer, MAX_STRING_SIZE, stdin) == NULL)
			return false;

		// removing newline
		buffer[strcspn(buffer, "\n")] = '\0';

		if (sanitized(buffer)) {
			strncpy(output, buffer, MAX_STRING_SIZE);
			return true;
		}

		fprintf(stderr, "Invalid input!\n\n");
	}
}

int main(int argc, char **argv)
{
	int fd;
	int option;
	long input;
	bool cache_exists;
	bool display_last = false;
	bool custom_param = false;
	size_t len;
	size_t thread;
	struct tst_tree **trees;
	char *dict;
	char *endptr;
	char hangman[MAX_STRING_SIZE];
	char wrong[MAX_STRING_SIZE];

	// default arguments
	bool use_cache = false;
	size_t threads = DEFAULT_WORKERS;
	char const *dict_file = DEFAULT_DICTIONARY;

	// getting arguments
	while ((option = getopt(argc, argv, "t:f:h:c")) != -1) {

		switch (option) {

		case 't':
			custom_param = true;

			// getting number of threads
			input = strtol(optarg, &endptr, 10);
			if ((input < 1) || (*endptr != '\0')
			    || (input > LONG_MAX)) {
				fprintf(stderr, INVALID_WORKERS, optarg);
				return EXIT_FAILURE;
			}

			threads = input;
			break;

		case 'f':
			custom_param = true;

			dict_file = optarg;
			break;

		case 'c':
			use_cache = true;
			break;

		default:
			fprintf(stderr, USAGE, argv[0]);
			return EXIT_FAILURE;
		}
	}

	cache_exists = (access(CACHE, F_OK) == 0);

	// should we load from cache or from a dictionary file
	if (use_cache && cache_exists && !custom_param) {
		fprintf(stderr, "Loading word list from cache file...\n\n");

		fd = e_open(CACHE, O_RDONLY);
		trees = tst_load_cache(fd, &threads);

	} else {
		fd = e_open(dict_file, O_RDONLY);

		// getting size of dictionary file
		len = end_lseek(fd);
		start_lseek(fd);

		// reading dictionary from file into memory
		dict = e_malloc(len + 1);
		e_read(fd, dict, len);
		dict[len] = '\0';

		trees = initialize_words(dict, threads);
		free(dict);
	}

	e_close(fd);

	if (use_cache && (custom_param || !cache_exists)) {
		fprintf(stderr, "Save cache file...\n\n");

		fd = e_creat(CACHE, CACHE_PERM);
		tst_save_cache(trees, threads, fd);
		e_close(fd);
	}

	// user input loop
	while (true) {

		if (!get_user_input("Hangman string: ", hangman, display_last))
			break;

		if (display_last)
			printf("\n");

		if (!get_user_input("Wrong letters:  ", wrong, display_last))
			break;

		fork_search(trees, hangman, wrong, threads);

		printf("\n\n");

		display_last = true;
	}

	printf("\n");

	// freeing memory, if cache was used, the memory is one contiguous block
	if (use_cache && cache_exists)
		free(*trees);
	else
		for (thread = 0; thread < threads; thread++)
			free(trees[thread]);

	free(trees);

	return EXIT_SUCCESS;
}

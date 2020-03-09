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

/* returns array of ternary search trees based on the number of
worker threads */
tst_node **initialize_words(FILE *word_list, size_t worker_threads)
{
	size_t len;
	char *str;
	char str_buffer[MAX_STRING_SIZE];
	d_array *array = d_array_create(ARRAY_START_SIZE);
	tst_node **roots;
	size_t idx;
	
	// reading file
	while (fgets(str_buffer, MAX_STRING_SIZE, word_list))
	{
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
	roots = e_malloc(worker_threads * sizeof(tst_node *));
	
	for (idx = 0; idx < worker_threads; idx++)
	{
		str = array->array[idx];
		len = strlen(str);
		roots[idx] = tst_create(str, 0, len);
	}
	
	// seperating array into separate ternary search trees
	// starting after the first words used to initialize the tst's
	for (idx = worker_threads; idx < array->elements; idx++)
	{
		str = array->array[idx];
		len = strlen(str);
		tst_insert(roots[idx % worker_threads], str, 0, len);
	}
	
	// freeing array memory
	d_array_destroy(array);
	
	return roots;
}

int main(int argc, char **argv)
{
	FILE *word_list;
	size_t worker_threads;
	size_t idx;
	int error;
	tst_node **roots;
	
	// checking arguments
	if (argc == 2)
	{
		// no word list given, using unix
		if (!(word_list = fopen("/usr/share/dict/words", "r"))) {
			perror("fopen");
			fprintf(stderr, "Unix word list not found.  Please "
					"give path to word list as argument.\n");
			exit(EXIT_FAILURE);
		}
	} else if (argc == 3)
	{
		// using word list specified
		if (!(word_list = fopen(argv[2], "r")))
		{
			perror("fopen");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		fprintf(stderr, "usage: %s worker_threads [path_to_word_list]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	// getting number of worker threads
	if (!(error = sscanf(argv[1], "%zu", &worker_threads)))
	{
		perror("scanf");
		exit(EXIT_FAILURE);
	}
	
	// initialization
	roots = initialize_words(word_list, worker_threads);
	fclose(word_list);
	
	// user input loop
	
	// freeing memory
	for (idx = 0; idx < worker_threads; idx++)
	{
		tst_destroy(roots[idx]);
	}
	free(roots);
}

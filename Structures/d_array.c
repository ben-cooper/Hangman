#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "d_array.h"

#define SCALE 2
#define PRINT_LINE_LIMIT 80
#define PRINT_ITEM_LIMIT 200
#define DELIM "  "
#define DELIM_LENGTH 2

d_array *d_array_create(size_t size)
{
	d_array *result;

	result = (d_array *) e_malloc(sizeof(d_array));
	result->array = (char **)e_malloc(size * sizeof(char *));
	result->elements = 0;
	result->capacity = size;

	return result;
}

void d_array_insert(d_array * array, char *item)
{
	/* checking if array is full */
	if (array->elements >= array->capacity) {
		/* increase size */
		array->array =
		    (char **)e_realloc(array->array,
				       array->elements * sizeof(char *) *
				       SCALE);
		array->capacity *= SCALE;
	}
	/* inserting element */
	array->array[array->elements] = item;
	array->elements++;
}

void d_array_destroy(d_array * array)
{
	free(array->array);
	free(array);
}

void d_array_print(d_array * array)
{
	size_t idx;
	size_t line_chars = 0;

	for (idx = 0; idx < array->elements; idx++) {
		
		line_chars += strlen(array->array[idx]) + DELIM_LENGTH;
		
		if (line_chars > PRINT_LINE_LIMIT) {
			printf("\n");
			line_chars = strlen(array->array[idx]) + DELIM_LENGTH;
		}

		printf("%s" DELIM, array->array[idx]);
		
		if (idx > PRINT_ITEM_LIMIT) {
			printf("\nPrint Limit reached!");
			break;
		}
	}
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "d_array.h"

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

void d_array_destroy(d_array *array, int destroy_str)
{
	size_t idx;
	
	if (destroy_str) {
		for (idx = 0; idx < array->elements; idx++) {
			free(array->array[idx]);
		}
	}
	
	free(array->array);
	free(array);
}

void d_array_print(d_array const *array)
{
	size_t idx;
	size_t col = 0;
	size_t row = 0;

	for (idx = 0; idx < array->elements; idx++) {
		
		col += strlen(array->array[idx]) + DELIM_LENGTH;

		/* checking end of column */
		if (col > PRINT_COL_LIMIT) {
			printf("\n");
			col = strlen(array->array[idx]) + DELIM_LENGTH;
			row++;
		}
		
		/* checking end of rows */
		if (row > PRINT_ROW_LIMIT) {
			printf("\nPrint Limit reached!");
			break;
		} else {
			printf("%s" DELIM, array->array[idx]);
		}
	}

	printf("\n");
}

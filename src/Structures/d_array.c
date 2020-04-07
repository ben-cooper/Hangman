#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "d_array.h"

struct d_array *d_array_create(size_t size)
{
	struct d_array *result;

	result = e_malloc(sizeof(struct d_array));
	result->array = e_malloc(size * sizeof(char *));
	result->elements = 0;
	result->capacity = size;

	return result;
}

void d_array_insert(struct d_array *array, char *item)
{
	/* if array is full, increase capacity */
	if (array->elements >= array->capacity) {
		array->capacity *= SCALE;
		array->array = e_realloc(array->array,
								array->capacity * sizeof(char *));
	}

	array->array[array->elements] = item;
	array->elements++;
}

void d_array_destroy(struct d_array *array, int destroy_str)
{
	size_t i;

	if (destroy_str)
		for (i = 0; i < array->elements; i++)
			free(array->array[i]);

	free(array->array);
	free(array);
}

void d_array_print(struct d_array const *array)
{
	size_t i;
	size_t col = 0;
	size_t row = 0;

	for (i = 0; i < array->elements; i++) {

		col += strlen(array->array[i]) + DELIM_LENGTH;

		if (col > PRINT_COL_LIMIT) {
			printf("\n");
			col = strlen(array->array[i]) + DELIM_LENGTH;
			row++;
		}

		if (row > PRINT_ROW_LIMIT) {
			printf("\nPrint Limit reached!");
			break;
		} else {
			printf("%s" DELIM, array->array[i]);
		}
	}

	printf("\n");
}

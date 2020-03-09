#include <stdlib.h>
#include "common.h"
#include "d_array.h"

#define SCALE 2

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
	// checking if array is full
	if (array->elements >= array->capacity) {
		// increase size
		array->array =
		    (char **)e_realloc(array->array,
				       array->elements * sizeof(char *) *
				       SCALE);
		array->capacity *= SCALE;
	}
	// inserting element
	array->array[array->elements] = item;
	array->elements++;
}

void d_array_destroy(d_array * array)
{
	free(array->array);
	free(array);
}

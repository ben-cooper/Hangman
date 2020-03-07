#include "common.h"

#define SCALE 2

typedef struct {
	char **array;
	size_t elements;
	size_t capacity;
} d_array;


d_array *d_array_create(size_t size)
{
	d_array *result;
	
	result = (d_array *) e_malloc(sizeof(d_array));
	result->array = (char **) e_malloc(size);
	result->elements = 0;
	result->capacity = size;
	
	return result;
}


void d_array_insert(d_array *array, char const *item)
{
	// checking if array is full
	if (array->elements >= array->capacity)
	{
		// increase size
		array->array = (char **) e_realloc(array->array, array->capacity * SCALE);
		array->capacity *= SCALE;
	}
	
	// inserting element
	array->array[array->elements] = item;
	array->elements++;
}

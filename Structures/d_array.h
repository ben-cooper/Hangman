#ifndef D_ARRAY_H
#define D_ARRAY_H

typedef struct {
	char **array;
	size_t elements;
	size_t capacity;
} d_array;

d_array *d_array_create(size_t size);
void d_array_insert(d_array * array, char *item);
void d_array_destroy(d_array * array);
void d_array_print(d_array * array);

#endif

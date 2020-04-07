#ifndef D_ARRAY_H
#define D_ARRAY_H

#define SCALE 2
#define PRINT_COL_LIMIT 80
#define PRINT_ROW_LIMIT 15
#define DELIM "  "
#define DELIM_LENGTH 2

/**
 * Structure for a dynamically expandable array
 */
struct d_array {
	char **array;
	size_t elements;
	size_t capacity;
};

/**
 * Returns a pointer to a new dynamic array
 * @param size: the starting size of the dynamic array
 */
struct d_array *d_array_create(size_t size);

/**
 * Inserts a new element to the end of the array
 * @param array: pointer to the array to insert into
 * @param item: the string to insert into the arrray
 */
void d_array_insert(struct d_array *array, char *item);

/**
 * Frees all memory used by the array
 * @param array: pointer to the array to destroy
 * @param destroy_str: flag, if set will free all strings in the array
 */
void d_array_destroy(struct d_array *array, int destroy_str);

/**
 * Prints the strings contained within the array within the bounds
 * of PRINT_COL_LIMIT and PRINT_ROW_LIMIT
 * @param array: pointer to the array to print
 */
void d_array_print(struct d_array const *array);

#endif

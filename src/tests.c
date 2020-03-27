#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "Structures/common.h"
#include "Structures/d_array.h"
#include "Structures/tst.h"
#include "Misc/misc.h"

/* minunit code */
#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { char *message = test(); tests_run++; \
                                if (message) return message; } while (0)

int tests_run = 0;
char *test_name;

/* ternary search tree tests */
char *test_tst_single_char_create()
{
	tst_node *root;
	test_name = "tst single character creation";

	root = tst_create("a", 0, 1);
	
	mu_assert("character not added", tst_search(root, "a", 0, 1));
	mu_assert("wrong character not added", !tst_search(root, "b", 0, 1));
	mu_assert("incorrect number of characters", tst_height(root) == 1);
	
	tst_destroy(root, 0);
	
	return NULL;
}

char *test_tst_multi_char_create()
{
	tst_node *root;
	test_name = "tst multi character creation";

	root = tst_create("cat", 0, 3);
	
	mu_assert("word not added", tst_search(root, "cat", 0, 3));
	mu_assert("bad words added", !tst_search(root, "at", 0, 2));
	mu_assert("bad words added", !tst_search(root, "t", 0, 1));
	mu_assert("incorrect number of characters", tst_height(root) == 3);

	tst_destroy(root, 0);

	return NULL;
}

char *test_tst_new_word_insert()
{
	tst_node *root;
	test_name = "tst new word insert";

	root = tst_create("hello", 0, 5);
	
	tst_insert(root, "testing", 0, 7);
	mu_assert("new word not found", tst_search(root, "testing", 0, 7));
	mu_assert("old word affected", tst_search(root, "hello", 0, 5));
	mu_assert("incorrect number of characters", tst_height(root) == 8);
	
	tst_destroy(root, 0);
	
	return NULL;
}

char *test_tst_substring_insert()
{
	tst_node *root;
	test_name = "tst substring insert";

	root = tst_create("hello", 0, 5);
	
	tst_insert(root, "hell", 0, 4);
	mu_assert("new word not found", tst_search(root, "hell", 0, 4));
	mu_assert("old word affected", tst_search(root, "hello", 0, 5));
	mu_assert("incorrect word added", !tst_search(root, "he", 0, 5));
	mu_assert("incorrect number of characters", tst_height(root) == 5);
	
	tst_destroy(root, 0);
	
	return NULL;
}

char *test_tst_partial_substring_insert()
{
	tst_node *root;
	test_name = "tst partial substring insert";

	root = tst_create("hello", 0, 5);
	
	tst_insert(root, "helper", 0, 6);
	mu_assert("new word not found", tst_search(root, "helper", 0, 6));
	mu_assert("old word affected", tst_search(root, "hello", 0, 5));
	mu_assert("incorrect word added", !tst_search(root, "he", 0, 5));
	mu_assert("incorrect number of characters", tst_height(root) == 7);
	
	tst_destroy(root, 0);
	
	return NULL;
}

char *test_tst_single_char_pattern_search()
{
	tst_node *root;
	int fd[2];
	char *output;
	
	test_name = "tst single char pattern search";
	
	pipe(fd);
	root = tst_create("a", 0, 1);
	tst_insert(root, "b", 0, 1);
	
	tst_pattern_search(root, "_", 0, 1, "", fd[1]);
	e_read(fd[0], &output, sizeof(output));
	mu_assert("first character not found", strcmp(output, "b") == 0);
	
	e_read(fd[0], &output, sizeof(output));
	mu_assert("second character not found", strcmp(output, "a") == 0);
	
	tst_destroy(root, 0);
	
	close(fd[0]);
	close(fd[1]);
	
	return NULL;
}

char *test_tst_pattern_search_valid_string()
{
	tst_node *root;
	int fd[2];
	char *output;
	ssize_t nbytes;
	
	test_name = "tst pattern search valid string";
	
	pipe(fd);
	root = tst_create("best", 0, 4);
	tst_insert(root, "rest", 0, 4);
	tst_insert(root, "resting", 0, 7);
	
	
	tst_pattern_search(root, "_est", 0, 4, "", fd[1]);
	e_read(fd[0], &output, sizeof(output));
	mu_assert("first string not found", strcmp(output, "rest") == 0);
	
	e_read(fd[0], &output, sizeof(output));
	mu_assert("second string not found", strcmp(output, "best") == 0);
	
	/* non blocking read to check empty pipe */
	if (fcntl(fd[0], F_SETFL, O_NONBLOCK) < 0)
	{
		perror("fcntl");
		exit(EXIT_FAILURE);
	}
	
	nbytes = read(fd[0], &output, sizeof(output));
	mu_assert("extra string added", (nbytes == -1) && (errno == EAGAIN));
	
	tst_destroy(root, 0);
	
	close(fd[0]);
	close(fd[1]);
	
	return NULL;
}

char *test_tst_pattern_search_invalid_string()
{
	tst_node *root;
	int fd[2];
	char *output;
	ssize_t nbytes;
	
	test_name = "tst pattern search valid string";
	
	pipe(fd);
	root = tst_create("best", 0, 4);
	tst_insert(root, "rest", 0, 4);
	tst_insert(root, "resting", 0, 7);
	
	
	tst_pattern_search(root, "_est_", 0, 5, "", fd[1]);
	
	/* non blocking read to check empty pipe */
	if (fcntl(fd[0], F_SETFL, O_NONBLOCK) < 0)
	{
		perror("fcntl");
		exit(EXIT_FAILURE);
	}
	
	nbytes = read(fd[0], &output, sizeof(output));
	mu_assert("string found", (nbytes == -1) && (errno == EAGAIN));
	
	tst_destroy(root, 0);
	
	close(fd[0]);
	close(fd[1]);
	
	return NULL;
}

/* dynamic array tests */
char *test_array_creation()
{
	d_array *array;
	test_name = "array creation";
	
	array = d_array_create(5);
	
	mu_assert("incorrect elements", array->elements == 0);
	mu_assert("incorrect capacity", array->capacity == 5);
	
	d_array_destroy(array, 0);
	
	return NULL;
}
	
char *test_array_normal_insert()
{
	d_array *array;
	test_name = "array normal insert";
	
	array = d_array_create(5);
	
	d_array_insert(array, "testing1");
	mu_assert("size not updated", array->elements == 1);
	mu_assert("incorrect capacity", array->capacity == 5);
	mu_assert("element not added", strcmp(array->array[0], "testing1") == 0);
	
	d_array_destroy(array, 0);
	
	return NULL;
}

char *test_array_multiple_insert()
{
	d_array *array;
	test_name = "array multiple insert";
	
	array = d_array_create(5);
	
	d_array_insert(array, "testing1");
	d_array_insert(array, "testing2");
	mu_assert("size not updated", array->elements == 2);
	mu_assert("capacity incorrect", array->capacity == 5);
	mu_assert("element not added", strcmp(array->array[0], "testing1") == 0);
	mu_assert("element not added", strcmp(array->array[1], "testing2") == 0);
	
	d_array_destroy(array, 0);
	
	return NULL;
}

char *test_array_insert_expansion()
{
	d_array *array;
	test_name = "array insert expansion";
	
	array = d_array_create(2);
	
	d_array_insert(array, "testing1");
	d_array_insert(array, "testing2");
	d_array_insert(array, "testing3");
	mu_assert("size not updated", array->elements == 3);
	mu_assert("capacity incorrect", array->capacity == 2 * SCALE);
	mu_assert("element not added", strcmp(array->array[0], "testing1") == 0);
	mu_assert("element not added", strcmp(array->array[1], "testing2") == 0);
	mu_assert("element not added", strcmp(array->array[2], "testing3") == 0);
	
	d_array_destroy(array, 0);
	
	return NULL;
}

/* sanitize tests */
char *test_sanitize_empty_string()
{
	test_name = "sanitize empty string";
	
	mu_assert("empty string passed", !sanitize("", ""));
	mu_assert("empty string passed with exception", !sanitize("", "_"));
	
	return NULL;
}

char *test_sanitize_clean_character()
{
	test_name = "sanitize clean character";
	
	mu_assert("single character failed", sanitize("a", ""));
	mu_assert("single character failed with exception", sanitize("a", "_"));
	
	return NULL;
}

char *test_sanitize_bad_character()
{
	test_name = "sanitize bad character";
	
	mu_assert("bad character passed", !sanitize("?", ""));
	mu_assert("bad character passed with exception", !sanitize("?", "_"));
	
	return NULL;
}

char *test_sanitize_newline_character()
{
	char str[] = "\n";
	int result = sanitize(str, "");
	
	test_name = "sanitize newline character";
	
	mu_assert("newline character passed", !result);
	mu_assert("newline character not removed", str[0] == '\0');
	
	return NULL;
}

char *test_sanitize_clean_string()
{
	test_name = "sanitize clean string";
	
	mu_assert("clean string failed", sanitize("hello", ""));
	mu_assert("clean string failed with exception", sanitize("hello", "_"));
	
	return NULL;
}

char *test_sanitize_bad_string()
{
	test_name = "sanitize bad string";
	
	mu_assert("bad string passed", !sanitize("hello5", ""));
	mu_assert("bad string passed with exception", !sanitize("hello5", "_"));
	mu_assert("bad string failed with good exception", sanitize("hello5", "5"));
	
	return NULL;
}

char *test_sanitize_clean_string_newline()
{
	char str[] = "testing\n";
	int result = sanitize(str, "");
	
	test_name = "sanitize clean string with newline";
	
	mu_assert("clean string failed", result);
	mu_assert("newline not removed", str[7] == '\0');
	
	return NULL;
}

char *test_sanitize_bad_string_newline()
{
	char str[] = "test?ing\n";
	int result1 = sanitize(str, "");
	int result2 = sanitize(str, "?");
	
	test_name = "sanitize bad string with newline";
	mu_assert("bad string passed", !result1);
	mu_assert("bad string failed with good exception", result2);
	mu_assert("newline not removed", str[8] == '\0');
	
	return NULL;
}

/* word analyzer tests */
extern void insertion_sort(double *values, char *keys, size_t len);

int helper_is_sorted(double *array, size_t len)
{
	size_t i;
	
	for (i = 0; i < (len - 1); i++) {
		if (array[i] < array[i+1]) {
			return 0;
		}
	}
	
	return 1;
}

char *test_insertion_sort_single_item()
{
	double values[1] = {5.0};
	char keys[1] = {'f'};

	test_name = "insertion sort single item";
	insertion_sort(values, keys, 1);
	
	mu_assert("value has changed", values[0] == 5.0);
	mu_assert("key has changed", keys[0] == 'f');
	
	return NULL;
}

char *test_insertion_sort_many_items()
{
	double values[5] = {3.0, 2.0, 4.0, 5.0, 1.0};
	char keys[5] = {'n', 'u', 'i', 'l', 'x'};
	
	test_name = "insertion sort many items";
	insertion_sort(values, keys, 5);
	
	mu_assert("values not sorted", helper_is_sorted(values, 5));
	mu_assert("keys not sorted", strncmp(keys, "linux", 5) == 0);
	
	return NULL;
}

char *all_tests()
{
	mu_run_test(test_tst_single_char_create);
	mu_run_test(test_tst_multi_char_create);
	mu_run_test(test_tst_new_word_insert);
	mu_run_test(test_tst_substring_insert);
	mu_run_test(test_tst_partial_substring_insert);
	mu_run_test(test_tst_single_char_pattern_search);
	mu_run_test(test_tst_pattern_search_valid_string);
	mu_run_test(test_tst_pattern_search_invalid_string);
	mu_run_test(test_array_creation);
	mu_run_test(test_array_normal_insert);
	mu_run_test(test_array_multiple_insert);
	mu_run_test(test_array_insert_expansion);
	mu_run_test(test_sanitize_empty_string);
	mu_run_test(test_sanitize_clean_character);
	mu_run_test(test_sanitize_bad_character);
	mu_run_test(test_sanitize_newline_character);
	mu_run_test(test_sanitize_clean_string);
	mu_run_test(test_sanitize_bad_string);
	mu_run_test(test_sanitize_clean_string_newline);
	mu_run_test(test_sanitize_bad_string_newline);
	mu_run_test(test_insertion_sort_single_item);
	mu_run_test(test_insertion_sort_many_items);
	return NULL;
}

int main()
{
	char *result = all_tests();
	
	if (result) {
		printf("%s: %s\n", test_name, result);
	} else {
		printf("All tests passed!\n");
	}
	
	printf("Tests run: %d\n", tests_run);
	
	return !result;
}

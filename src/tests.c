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
	struct tst_node *root;
	test_name = "tst single character creation";

	root = tst_create("a", 0, 1);

	mu_assert("character not added", tst_search(root, "a", 0, 1));
	mu_assert("wrong character not added", !tst_search(root, "b", 0, 1));
	mu_assert("incorrect number of characters", tst_height(root) == 1);

	tst_destroy(root);

	return NULL;
}

char *test_tst_multi_char_create()
{
	struct tst_node *root;
	test_name = "tst multi character creation";

	root = tst_create("cat", 0, 3);

	mu_assert("word not added", tst_search(root, "cat", 0, 3));
	mu_assert("bad words added", !tst_search(root, "at", 0, 2));
	mu_assert("bad words added", !tst_search(root, "t", 0, 1));
	mu_assert("incorrect number of characters", tst_height(root) == 3);

	tst_destroy(root);

	return NULL;
}

char *test_tst_new_word_insert()
{
	struct tst_node *root;
	test_name = "tst new word insert";

	root = tst_create("hello", 0, 5);

	tst_insert(root, "testing", 0, 7);
	mu_assert("new word not found", tst_search(root, "testing", 0, 7));
	mu_assert("old word affected", tst_search(root, "hello", 0, 5));
	mu_assert("incorrect number of characters", tst_height(root) == 8);

	tst_destroy(root);

	return NULL;
}

char *test_tst_substring_insert()
{
	struct tst_node *root;
	test_name = "tst substring insert";

	root = tst_create("hello", 0, 5);

	tst_insert(root, "hell", 0, 4);
	mu_assert("new word not found", tst_search(root, "hell", 0, 4));
	mu_assert("old word affected", tst_search(root, "hello", 0, 5));
	mu_assert("incorrect word added", !tst_search(root, "he", 0, 5));
	mu_assert("incorrect number of characters", tst_height(root) == 5);

	tst_destroy(root);

	return NULL;
}

char *test_tst_partial_substring_insert()
{
	struct tst_node *root;
	test_name = "tst partial substring insert";

	root = tst_create("hello", 0, 5);

	tst_insert(root, "helper", 0, 6);
	mu_assert("new word not found", tst_search(root, "helper", 0, 6));
	mu_assert("old word affected", tst_search(root, "hello", 0, 5));
	mu_assert("incorrect word added", !tst_search(root, "he", 0, 5));
	mu_assert("incorrect number of characters", tst_height(root) == 7);

	tst_destroy(root);

	return NULL;
}

char *test_tst_single_char_pattern_search()
{
	struct tst_node *root;
	int fd[2];
	char output[2] = { '\0' };
	char buffer[1];

	test_name = "tst single char pattern search";

	pipe(fd);
	root = tst_create("a", 0, 1);
	tst_insert(root, "b", 0, 1);

	tst_pattern_search(root, "-", 0, 1, "", fd[1], buffer);
	e_read(fd[0], output, 1);
	mu_assert("first character not found", strcmp(output, "b") == 0);

	e_read(fd[0], output, 1);
	mu_assert("second character not found", strcmp(output, "a") == 0);

	tst_destroy(root);

	close(fd[0]);
	close(fd[1]);

	return NULL;
}

char *test_tst_pattern_search_valid_string()
{
	struct tst_node *root;
	int fd[2];
	char output1[8] = { '\0' };
	char output2[8] = { '\0' };
	char buffer[7];
	ssize_t nbytes;

	test_name = "tst pattern search valid string";

	pipe(fd);
	root = tst_create("best", 0, 4);
	tst_insert(root, "rest", 0, 4);
	tst_insert(root, "resting", 0, 7);


	tst_pattern_search(root, "-est", 0, 4, "", fd[1], buffer);
	e_read(fd[0], output1, 4);
	e_read(fd[0], output2, 4);

	mu_assert("valid strings not found",
	          ((strcmp(output1, "best") == 0) && (strcmp(output2, "rest") == 0)) ||
	          ((strcmp(output1, "rest") == 0) && (strcmp(output2, "best") == 0)));

	/* non blocking read to check empty pipe */
	fcntl(fd[0], F_SETFL, O_NONBLOCK);

	nbytes = read(fd[0], output1, 7);
	mu_assert("extra string added", (nbytes == -1) && (errno == EAGAIN));

	tst_destroy(root);

	close(fd[0]);
	close(fd[1]);

	return NULL;
}

char *test_tst_pattern_search_invalid_string()
{
	struct tst_node *root;
	int fd[2];
	char output[8] = { '\0' };
	char buffer[7];
	ssize_t nbytes;

	test_name = "tst pattern search valid string";

	pipe(fd);
	root = tst_create("best", 0, 4);
	tst_insert(root, "rest", 0, 4);
	tst_insert(root, "resting", 0, 7);

	tst_pattern_search(root, "-est-", 0, 5, "", fd[1], buffer);

	/* non blocking read to check empty pipe */
	fcntl(fd[0], F_SETFL, O_NONBLOCK);

	nbytes = read(fd[0], output, 8);
	mu_assert("string found", (nbytes == -1) && (errno == EAGAIN));

	tst_destroy(root);

	close(fd[0]);
	close(fd[1]);

	return NULL;
}

char *test_tst_pattern_search_duplicate_unknown_letter()
{
	struct tst_node *root;
	int fd[2];
	char output1[7] = { '\0' };
	char output2[7] = { '\0' };
	char buffer[6];
	ssize_t nbytes;

	test_name = "tst pattern search duplicate unknown letter";

	pipe(fd);
	root = tst_create("bitter", 0, 6);
	tst_insert(root, "better", 0, 6);
	tst_insert(root, "butter", 0, 6);

	tst_pattern_search(root, "b-tter", 0, 6, "", fd[1], buffer);

	e_read(fd[0], output1, 6);
	e_read(fd[0], output2, 6);

	mu_assert("valid strings not found",
	          ((strcmp(output1, "bitter") == 0) && (strcmp(output2, "butter") == 0)) ||
	          ((strcmp(output1, "butter") == 0) && (strcmp(output2, "bitter") == 0)));

	/* non blocking read to check empty pipe */
	fcntl(fd[0], F_SETFL, O_NONBLOCK);

	nbytes = read(fd[0], &output1, sizeof(output1));
	mu_assert("extra string added", (nbytes == -1) && (errno == EAGAIN));

	tst_destroy(root);

	close(fd[0]);
	close(fd[1]);

	return NULL;
}

/* dynamic array tests */
char *test_array_creation()
{
	struct d_array *array;
	test_name = "array creation";

	array = d_array_create(5);

	mu_assert("incorrect elements", array->elements == 0);
	mu_assert("incorrect capacity", array->capacity == 5);

	d_array_destroy(array, 0);

	return NULL;
}

char *test_array_normal_insert()
{
	struct d_array *array;
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
	struct d_array *array;
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
	struct d_array *array;
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

/* sanitized tests */
char *test_sanitized_empty_string()
{
	test_name = "sanitized empty string";

	mu_assert("empty string passed", !sanitized("", ""));
	mu_assert("empty string passed with exception", !sanitized("", "_"));

	return NULL;
}

char *test_sanitized_clean_character()
{
	test_name = "sanitized clean character";

	mu_assert("single character failed", sanitized("a", ""));
	mu_assert("single character failed with exception", sanitized("a", "_"));

	return NULL;
}

char *test_sanitized_bad_character()
{
	test_name = "sanitized bad character";

	mu_assert("bad character passed", !sanitized("?", ""));
	mu_assert("bad character passed with exception", !sanitized("?", "_"));

	return NULL;
}

char *test_sanitized_clean_string()
{
	test_name = "sanitized clean string";

	mu_assert("clean string failed", sanitized("hello", ""));
	mu_assert("clean string failed with exception", sanitized("hello", "_"));

	return NULL;
}

char *test_sanitized_bad_string()
{
	test_name = "sanitized bad string";

	mu_assert("bad string passed", !sanitized("hello5", ""));
	mu_assert("bad string passed with exception", !sanitized("hello5", "_"));
	mu_assert("bad string failed with good exception", sanitized("hello5", "5"));

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
	mu_run_test(test_tst_pattern_search_duplicate_unknown_letter);
	mu_run_test(test_array_creation);
	mu_run_test(test_array_normal_insert);
	mu_run_test(test_array_multiple_insert);
	mu_run_test(test_array_insert_expansion);
	mu_run_test(test_sanitized_empty_string);
	mu_run_test(test_sanitized_clean_character);
	mu_run_test(test_sanitized_bad_character);
	mu_run_test(test_sanitized_clean_string);
	mu_run_test(test_sanitized_bad_string);

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

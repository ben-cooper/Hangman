#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

extern void print_probabilities(char **str, size_t n);
extern void shuffle(char **strs, size_t n);
extern void sanitize(char *str, size_t len);

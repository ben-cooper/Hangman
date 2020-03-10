#ifndef MISC_H
#define MISC_H

void print_probabilities(char *const *str, size_t n);
void shuffle(char **strs, size_t n);
void sanitize(char *str, size_t len, char const * exceptions);

#endif

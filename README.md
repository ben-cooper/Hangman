# Hangman

A small program to assist in solving hangman puzzles.  This program can
list all possible words (if any) and provide the frequency of each letter
in the list possible words to provide the best guesses statistically.

## Compilation

For normal usage:

`make`

To remove compiled files:

`make clean`

Environment Variables:
* `LLVM`: compiles using clang and LLVM utilities when set to 1
* `DEBUG`: disables optimizations and includes debugging information when set to 1
* `STATIC`: statically links dependencies into the binary when set to 1

### Examples

* `make DEBUG=1`
* `make LLVM=1 STATIC=1`

## Usage

`usage: ./hangman [-t worker-threads]  [-f path_to_word_list] [-c]`

`-t worker_threads` (optional) specifies the number of child processes used to
search the dictionary.  The default is `1`.

`-f path_to_word_list` (optional) specifies the path to an alternative word list
or dictionary.  The default is `/usr/share/dict/words`

`-c` (optional) creates a cache if none exists or if used in conjunction with
`-t` or `-f`, otherwise loads the word list from an existing cache (the cache
determines the number of threads)


During usage `CTRL-D` can be used to exit at either the hangman string prompt
or the wrong letters prompt.

### Example

```
$ ./hangman -t 4
Hangman string: -o--u-e-
Wrong letters:  ai

Possible words:
fortunes  colluded  colludes  colluder  commuted  commutes  commuter  communes
communed  computed  computes  computer  costumed  costumes  costumer  cornuted
corpuses  conduced  conduces  confused  confuses  confuted  confuter  confutes
conjures  conjurer  conjured  consumed  consumes  consumer  contused  contuses
tonsured  tonsures  tortured  tortures  torturer  postures  posturer  postured
pollutes  polluter  polluted

Word(s) found: 43

Letter probabilities:
Letter: c	Chance: 72.09%
Letter: t	Chance: 62.79%
Letter: s	Chance: 60.47%
Letter: n	Chance: 48.84%
Letter: r	Chance: 48.84%
Letter: d	Chance: 44.19%
Letter: m	Chance: 32.56%
Letter: p	Chance: 23.26%
Letter: f	Chance: 13.95%
Letter: l	Chance: 13.95%
Letter: j	Chance: 6.98%
```

## Configuration

For printing possible words, several constants can be altered in the file
`word_utilities.h` to adjust the format, including:

* `WILDCARD`: the character used to determine an unknown letter
* `PRINT_COL_LIMIT`: how many characters can be printed per line
* `PRINT_ROW_LIMIT`: how many lines of words can be printed
* `DELIM`: the string of characters to put between each possible word

For other compile-time options that affect system usage, constants can be
found in `hangman.c`.

* `MAX_STRING_SIZE`: the maximum number of characters in a word
* `DEFAULT_WORKERS`: the default number of child processes to use (must be >0)
* `DEFAULT_DICTIONARY`: the default path to the word list
* `CACHE`: the name of the cache file
* `CACHE_PERM`: the file permissions of the cache file

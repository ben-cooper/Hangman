# Hangman

A small program to assist in the solving of hangman puzzles.  This program
can list all possible solutions (if any) and provide the frequency of each
letter in the possible solutions to provide the best guesses statistically.

## Compilation

For normal usage: `make` or `make hangman`  
For unit testing: `make tests`  
To remove compiled files: `make clean`  

Environment Variables:
* CC: determines C compiler used and compiler specific optimizations
* DEBUG: disables optimizations and includes debugging information when set to 1

### Example

`make CC=clang DEBUG=1 tests`

## Usage

`usage: ./hangman [-t worker_threads]  [-f path_to_word_list]`  

`workers` specifies the number of workers threads (actually forks) used to
search the dictionary.  

`path_to_word_list` specifies the path to an alternative word list or
dictionary (optional).  The default is the Unix word list using the path.
`/usr/share/dict/words`  

During usage `CTRL-D` can be used to exit at either the hangman string prompt
or the wrong letters prompt.

### Example

```
$ ./hangman
Hangman string: -o--u-e-
Wrong letters: ai

Possible words:
fortunes  colluded  colludes  colluder  commuted  commutes  commuter  communes  
communed  computed  computes  computer  costumed  costumes  costumer  cornuted  
corpuses  conduced  conduces  confused  confuses  confuted  confuter  confutes  
conjures  conjurer  conjured  consumed  consumes  consumer  contused  contuses  
tonsured  tonsures  tortured  tortures  torturer  postures  posturer  postured  
pollutes  polluter  polluted  

Words found: 43

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

$ ./tests
All tests passed!
Tests run: 18
```

## Configuration

For printing possible words, several constants can be altered in the file
`d_array.h` to adjust the format, including:

* `PRINT_COL_LIMIT`: how many characters can be printed per line
* `PRINT_ROW_LIMIT`: how many lines of words can be printed

For adjusting options that affect memory usage, constant can be found in
`d_array.h` and `hangman.c`.  

* `SCALE`: the scaling factory for the dynamic array (d_array)
* `ARRAY_START_SIZE`: the starting size for the dynamic array
* `MAX_STRING_SIZE`: the maximum string size

## Dependencies

* libedit (development files)

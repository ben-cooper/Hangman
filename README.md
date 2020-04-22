# Hangman

A small program to assist in the solving of hangman puzzles.  This program
can list all possible solutions (if any) and provide the frequency of each
letter in the possible solutions to provide the best guesses statistically.

## Compilation

For normal usage: `make` or `make release`  
For debugging: `make debug`  
For unit testing: `make tests`  
To remove compiled files: `make clean`  


## Usage

`usage: ./hangman [-t worker_threads]  [-f path_to_word_list]`  

`workers` specifies the number of workers threads (actually forks) used to
search the dictionary.  

`path_to_word_list` specifies the path to an alternative word list or
dictionary (optional).  The default is the Unix word list using the path.
`/usr/share/dict/words`  

During usage `CTRL-D` can be used to exit at either the hangman string prompt
or the wrong letters prompt.

Thanks to the GNU Readline library, `CTRL-P` and `CTRL-N` can be used to
scroll through previous entries.  To learn more about its functionality
go to [https://tiswww.case.edu/php/chet/readline/rltop.html](https://tiswww.case.edu/php/chet/readline/rltop.html)  



## Examples

```
$ ./hangman
Hangman string: _o__u_e_
Wrong letters: ai

Possible words:
fortunes  colluded  colludes  colluder  commuted  commutes  commuter  communes  
communed  computed  computes  computer  costumed  costumes  costumer  cornuted  
corpuses  conduced  conduces  confused  confuses  confuted  confuter  confutes  
conjures  conjurer  conjured  consumed  consumes  consumer  contused  contuses  
tonsured  tonsures  tortured  tortures  torturer  postures  posturer  postured  
pollutes  polluter  polluted  

Total words found: 43

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
Tests run: 21
```

## Configuration

For printing possible words, several constants can be altered in the file
`d_array.h` to adjust the format, including:

* Column limit (how many characters can be printed per line)
* Row limit (how many lines of items can be printed.

For adjusting options that affect memory usage, constant can be found in
`d_array.h` and `hangman.c`.  

* Scaling factory for the dynamic array (d_array)
* Array starting size for the dynamic array
* Maximum string size

## Dependencies

* GNU Readline

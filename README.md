# Hangman

A small program to assist in the solving of hangman puzzles.  This program
can list all possible solutions (if any) and provide the frequency of each
letter in the possible solutions to provide the best guesses statistically.

## Compilation

For normal usage: `make`  
For debugging: `make debug`  
To optimize: `make fast`  


## Usage

`usage: ./hangman workers [path_to_word_list]`  

`workers` specifies the number of workers threads (actually forks) used to
search the dictionary.  

`path_to_word_list` specifies the path to an alternative word list or
dictionary (optional).  The default is the Unix word list using the path.
`/usr/share/dict/words`  

During usage `CTRL-D` can be used at either the hangman string prompt or
the wrong letters prompt.  

Thanks to the GNU Readline library, `CTRL-P` and `CTRL-N` can be used to
scroll through previous entries.  To learn more about it's functionality
go to [https://tiswww.case.edu/php/chet/readline/rltop.html](https://tiswww.case.edu/php/chet/readline/rltop.html)  



## Examples

```
$ ./hangman 3
Hangman string: ___
Wrong letters: eaoi

Possible words:
bus  bur  hub  huh  hut  gyp  gum  fry  jug  jut  dub  dug  nun  ply  pwn  pun  
pus  rut  mum  tug  tun  tux  sum  yum  wry  cut  cur  cry  dud  dun  dry  cup  
gnu  guy  gut  fly  fun  mug  nub  bud  bum  buy  rum  run  rub  ugh  ump  tub  
yuk  why  shy  sly  sup  sub  sty  spy  pyx  pry  brr  bug  bun  but  flu  fur  
gym  gun  duh  cub  cud  hug  hum  lug  mud  pub  pug  pup  put  rug  urn  ups  
thy  try  sky  sun  yup  nth  nut  

Letter Probabilities:
Letter: u	Chance: 75.86%
Letter: y	Chance: 26.44%
Letter: b	Chance: 19.54%
Letter: g	Chance: 19.54%
Letter: p	Chance: 19.54%
Letter: r	Chance: 18.39%
Letter: t	Chance: 18.39%
Letter: n	Chance: 17.24%
Letter: s	Chance: 13.79%
Letter: h	Chance: 12.64%
Letter: m	Chance: 12.64%
Letter: d	Chance: 10.34%
Letter: c	Chance: 6.90%
Letter: f	Chance: 5.75%
Letter: l	Chance: 5.75%
Letter: w	Chance: 3.45%
Letter: j	Chance: 2.30%
Letter: k	Chance: 2.30%
Letter: x	Chance: 2.30%


Hangman string:
```

## Configuration

For printing possible words, several constants can be altered in the file
`d_array.h` to adjust the format, including:

* Line character limit
* Item limit (how many words can be printed)

For adjusting options that affect memory usage, constant can be found in
`d_array.h` and `hangman.c`.  

* Scaling factory for the dynamic array (d_array)
* Array starting size for the dynamic array
* Maximum string size

## Dependencies

* GNU Readline

CC := gcc
CFLAGS := -Wall -Wextra -Werror --std=c99
LFLAGS := -lreadline
STRUCTURES := Structures/d_array.o Structures/tst.o Structures/common.o
MISC := Misc/sanitize.o Misc/shuffle.o Misc/word_analyzer.o
MAIN := ./hangman.o
OBJECTS := $(STRUCTURES) $(MISC) $(MAIN)

default: hangman

fast: CFLAGS += -O3
fast: hangman

debug: CFLAGS += -g
debug: hangman

hangman: $(OBJECTS)
	$(CC) $(LFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	
clean: 
	rm -f $(OBJECTS)

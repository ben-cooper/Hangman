CC := gcc
CFLAGS := -Wall -Wextra -Werror --std=c89
LIBS := -lreadline

OBJECTS := common.o d_array.o tst.o sanitize.o shuffle.o word_analyzer.o hangman.o
OBJECTS := $(addprefix obj/, $(OBJECTS))
BINARY := bin/hangman

.PHONY: clean

default: obj bin $(BINARY)

fast: CFLAGS += -O3
fast: default

debug: CFLAGS += -g
debug: default

obj:
	mkdir $@

bin:
	mkdir $@

$(BINARY): $(OBJECTS)
	$(CC) $(LIBS) $^ -o $@

obj/common.o: src/Structures/common.c
	$(CC) $(CFLAGS) -c $^ -o $@

obj/d_array.o: src/Structures/d_array.c
	$(CC) $(CFLAGS) -c $^ -o $@

obj/tst.o: src/Structures/tst.c
	$(CC) $(CFLAGS) -c $^ -o $@

obj/sanitize.o: src/Misc/sanitize.c
	$(CC) $(CFLAGS) -c $^ -o $@

obj/shuffle.o: src/Misc/shuffle.c
	$(CC) $(CFLAGS) -c $^ -o $@

obj/word_analyzer.o: src/Misc/word_analyzer.c
	$(CC) $(CFLAGS) -c $^ -o $@

obj/hangman.o: src/hangman.c
	$(CC) $(CFLAGS) -c $^ -o $@
	
clean:
	rm -f obj/* bin/*

CC := gcc
CFLAGS := -Wall -Wextra -Werror --std=c89
LFLAGS := -lreadline

STRUCTURES := d_array.o tst.o common.o
STRUCTURES := $(addprefix obj/Structures/, $(STRUCTURES))

MISC := sanitize.o shuffle.o word_analyzer.o
MISC := $(addprefix obj/Misc/, $(MISC))

MAIN := obj/hangman.o

OBJECTS := $(STRUCTURES) $(MISC) $(MAIN)

BINARY := bin/hangman

.PHONY: default fast debug small clean

default: $(BINARY)

fast: CFLAGS += -O2
fast: default

debug: CFLAGS += -g
debug: default

small: CFLAGS += -Os
small: LFLAGS += -fdata-sections -ffunction-sections -s -Wl,--gc-sections
small: default

$(BINARY): $(OBJECTS)
	mkdir -p $(@D)
	$(CC) $(LFLAGS) $^ -o $@

obj/Structures/%.o: src/Structures/%.c src/Structures/%.h
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

obj/Misc/%.o: src/Misc/%.c src/Misc/misc.h
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

obj/%.o: src/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -rf obj/ bin/

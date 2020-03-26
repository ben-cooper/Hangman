CC := gcc
CFLAGS := -Wall -Wextra -Werror --std=c89
LFLAGS := -lreadline

STRUCTURES := d_array.o tst.o common.o
STRUCTURES := $(addprefix obj/Structures/, $(STRUCTURES))

MISC := sanitize.o shuffle.o word_analyzer.o
MISC := $(addprefix obj/Misc/, $(MISC))

MAIN_OBJECTS := $(STRUCTURES) $(MISC) obj/hangman.o
TEST_OBJECTS := $(STRUCTURES) $(MISC) obj/tests.o

.PHONY: default fast debug small clean tests

default: bin/hangman

fast: CFLAGS += -O2
fast: default

debug: CFLAGS += -g
debug: default

small: CFLAGS += -Os -fdata-sections -ffunction-sections
small: LFLAGS += -Wl,--gc-sections -s
small: default

tests: CFLAGS += -g
tests: bin/tests

bin/hangman: $(MAIN_OBJECTS)
	mkdir -p $(@D)
	$(CC) $^ -o $@ $(LFLAGS)

bin/tests: $(TEST_OBJECTS)
	mkdir -p $(@D)
	$(CC) $^ -o $@ $(LFLAGS)

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

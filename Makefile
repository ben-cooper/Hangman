CFLAGS := -Wall -Wextra -Werror -Wpedantic --std=c89
LFLAGS := -ledit

OBJECTS := tst.o common.o word_utilities.o
OBJECTS := $(addprefix obj/, $(OBJECTS))

ifeq ($(DEBUG),1)
	CFLAGS += -g -Og
else
	ifeq ($(CC),clang)
		CFLAGS += -Oz -flto=full -march=native
		LFLAGS += -fuse-ld=lld -rtlib=compiler-rt -s
	else
		CFLAGS += -Os -flto -fdata-sections -ffunction-sections
		LFLAGS += -Wl,--gc-sections -s
	endif
endif

.PHONY: hangman tests clean

hangman: bin/hangman

tests: bin/tests

bin/hangman: $(OBJECTS) obj/hangman.o
	mkdir -p $(@D)
	$(CC) $^ -o $@ $(LFLAGS)

bin/tests: $(OBJECTS) obj/tests.o
	mkdir -p $(@D)
	$(CC) $^ -o $@ $(LFLAGS)

obj/%.o: src/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj/ bin/

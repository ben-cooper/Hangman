CFLAGS := -Wall -Wextra -Werror -Wpedantic --std=c89
LFLAGS := -ledit

STRUCT_OBJECTS := d_array.o tst.o common.o
STRUCT_OBJECTS := $(addprefix obj/Structures/, $(STRUCT_OBJECTS))

MISC_OBJECTS := sanitize.o shuffle.o word_analyzer.o
MISC_OBJECTS := $(addprefix obj/Misc/, $(MISC_OBJECTS))

ifeq ($(DEBUG),1)
	CFLAGS += -g -Og
else
	ifeq ($(CC),clang)
		CFLAGS += -Oz -flto=full -march=native
		LFLAGS += -fuse-ld=lld -rtlib=compiler-rt -s
	else
		CFLAGS += -Os -fdata-sections -ffunction-sections
		LFLAGS += -Wl,--gc-sections -s
	endif
endif

.PHONY: hangman tests clean

hangman: bin/hangman

tests: bin/tests

bin/hangman: $(STRUCT_OBJECTS) $(MISC_OBJECTS) obj/hangman.o
	mkdir -p $(@D)
	$(CC) $^ -o $@ $(LFLAGS)

bin/tests: $(STRUCT_OBJECTS) $(MISC_OBJECTS) obj/tests.o
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

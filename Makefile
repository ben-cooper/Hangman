CFLAGS := -Wall -Wextra -Werror -Wpedantic --std=c89 -fdata-sections -ffunction-sections
LFLAGS := -Wl,-gc-sections

OBJECTS := tst.o common.o word_utilities.o
OBJECTS := $(addprefix obj/, $(OBJECTS))

ifeq ($(DEBUG),1)
	CFLAGS += -g -Og
else
	ifeq ($(LLVM), 1)
		CC := clang
		CFLAGS += -Oz -flto=full -march=native
		LFLAGS += -fuse-ld=lld -rtlib=compiler-rt -s
	else
		CFLAGS += -Oz -flto
		LFLAGS += -s
	endif
endif

ifeq ($(STATIC),1)
	LFLAGS += -static
endif

.PHONY: hangman clean

hangman: bin obj bin/hangman

bin/hangman: $(OBJECTS) obj/hangman.o
	$(CC) $^ -o $@ $(LFLAGS)

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

bin:
	mkdir -p $@

obj:
	mkdir -p $@

clean:
	rm -rf obj/ bin/

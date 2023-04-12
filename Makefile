CC ?= gcc

CFLAGS = -Wall -Wextra -std=c17 -Isrc
LDFLAGS = -lm

SOURCE = $(wildcard src/*.c)
OBJECT = $(SOURCE:src/%.c=$(CC)_%.o)

HEADLESS_OBJECT = $(OBJECT:$(CC)_main_dcgi.o=)
DCGI_OBJECT = $(OBJECT:$(CC)_main_headless.o=)

HEADLESS = $(CC)_headless.out
DCGI = $(CC)_dcgi.out

.PHONY: clean fmt

all: $(HEADLESS) $(DCGI)

headless: $(HEADLESS)
dcgi: $(DCGI)

$(CC)_%.o: src/%.c
	$(CC) $(CFLAGS) $(EXT_CFLAGS) $(DEFS) -c $< -o $@

$(HEADLESS): $(HEADLESS_OBJECT)
	$(CC) $(HEADLESS_OBJECT) -o $(HEADLESS) $(LDFLAGS) $(EXT_LDFLAGS)

$(DCGI): $(DCGI_OBJECT)
	$(CC) $(DCGI_OBJECT) -o $(DCGI) $(LDFLAGS) $(EXT_LDFLAGS)

fmt:
	clang-format -i src/*

clean:
	-rm *.o *.out

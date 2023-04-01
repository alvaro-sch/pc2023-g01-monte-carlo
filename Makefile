CC ?= gcc

CFLAGS = -Wall -Wextra -std=c17 -Isrc
LDFLAGS = -lm

SOURCE = $(wildcard src/*.c)
OBJECT = $(SOURCE:src/%.c=$(CC)_%.o)

TARGET = $(CC).out

.PHONY: clean

all: $(TARGET)

fmt:
	clang-format -i src/*

$(CC)_%.o: src/%.c
	$(CC) $(CFLAGS) $(EXT_CFLAGS) $(DEFS) -c $< -o $@

$(TARGET): $(OBJECT)
	$(CC) $(OBJECT) -o $(TARGET) $(LDFLAGS) $(EXT_LDFLAGS)

clean:
	-rm *.o *.out

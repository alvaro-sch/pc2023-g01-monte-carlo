CC = gcc

CFLAGS = -Wall -Wextra -std=c17 -Isrc
LDFLAGS = -lm

T ?= debug

ifeq ($T,release)
CFLAGS += -O2 -DNDEBUG -march=native -ffast-math -funroll-loops
else ifeq ($T,debug)
CFLAGS +=
else
$(error invalid mode $T)
endif

SOURCE = $(wildcard src/*.c)
OBJECT = $(SOURCE:src/%.c=$T_%.o)

TARGET = $T.out

.PHONY: clean

all: $(TARGET)

fmt:
	clang-format -i src/*

$T_%.o: src/%.c
	$(CC) $(CFLAGS) $(DEFS) -c $< -o $@

$(TARGET): $(OBJECT)
	$(CC) $(OBJECT) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

clean:
	-rm *.o *.out

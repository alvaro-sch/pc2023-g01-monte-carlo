CC = gcc

CFLAGS = -Wall -Wextra -std=c17 -Isrc
LDFLAGS = -lm

T ?= debug

ifeq ($T,release)
CFLAGS += -O3 -DNDEBUG
else ifeq ($T,debug)
CFLAGS += -g -O0 -DDEBUG
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
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECT)
	$(CC) $(OBJECT) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

clean:
	-rm *.o *.out

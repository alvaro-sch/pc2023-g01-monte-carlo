CFLAGS = -Wall -Wextra -std=c17 -Iinclude -fopenmp
LDFLAGS = -lm -fopenmp

DCGI_LDFLAGS = `pkg-config --libs sdl2 glew`

SOURCE = $(wildcard src/*.c)
SHADER_SRC = $(wildcard shaders/*.vert shaders/*.frag)

OBJECT = $(SOURCE:src/%.c=$(CC)_%.o)
SHADER_OBJ = $(SHADER_SRC:shaders/%=%.o)

HEADLESS_OBJECT = $(OBJECT:$(CC)_main_dcgi.o=)
HEADLESS_OBJECT := $(HEADLESS_OBJECT:$(CC)_shader.o=)
DCGI_OBJECT = $(OBJECT:$(CC)_main_headless.o=) $(SHADER_OBJ)

HEADLESS = $(CC)_headless.out
DCGI = $(CC)_dcgi.out

.PHONY: clean fmt

all: $(HEADLESS) $(DCGI)

headless: $(HEADLESS)
dcgi: $(DCGI)

%.vert.o: shaders/%.vert
	ld -r -b binary -o $@ $<

%.frag.o: shaders/%.frag
	ld -r -b binary -o $@ $<

$(CC)_%.o: src/%.c
	$(CC) $(CFLAGS) $(EXT_CFLAGS) $(DEFS) -c $< -o $@

$(HEADLESS): $(HEADLESS_OBJECT)
	$(CC) $(HEADLESS_OBJECT) -o $(HEADLESS) $(LDFLAGS) $(EXT_LDFLAGS)

$(DCGI): $(DCGI_OBJECT)
	$(CC) $(DCGI_OBJECT) -o $(DCGI) $(LDFLAGS) $(EXT_LDFLAGS) $(DCGI_LDFLAGS)

fmt:
	clang-format -i src/*

clean:
	-rm *.o *.out

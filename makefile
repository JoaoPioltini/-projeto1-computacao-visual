TARGET = programa
SRC = src/main.c
PKGS = sdl3 sdl3-image sdl3-ttf

CC ?= gcc
CFLAGS = -std=c23 -Wall -Wextra -Wpedantic -Wno-unused-result
PKG_CFLAGS = $(shell pkg-config --cflags $(PKGS))
PKG_LIBS = $(shell pkg-config --libs $(PKGS))

.PHONY: all run clean check-deps

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(PKG_CFLAGS) $(SRC) -o $(TARGET) $(PKG_LIBS)

run: $(TARGET)
	./$(TARGET)

check-deps:
	pkg-config --modversion $(PKGS)

clean:
	$(RM) $(TARGET) output_image.png

TARGET = programa
SRC = src/main.c
PKGS = sdl3 sdl3-image sdl3-ttf

CC = gcc
CFLAGS = -std=c23 -Wall -Wextra -Wpedantic -Wno-unused-result

ifeq ($(OS),Windows_NT)
EXE_EXT = .exe
RUN_PREFIX =
SDL_DIR ?= d:/dev/compvis/libs/SDL3
SDL_INC_DIR = $(SDL_DIR)/include
SDL_LIB_DIR = $(SDL_DIR)/lib
SDL_BIN_DIR = $(SDL_DIR)/bin
INC_DIRS = -I$(SDL_INC_DIR)
LIB_DIRS = -L$(SDL_LIB_DIR)
LIBS = -lSDL3 -lSDL3_image -lSDL3_ttf -lm
else
EXE_EXT =
RUN_PREFIX = ./
INC_DIRS = $(shell pkg-config --cflags $(PKGS))
LIB_DIRS =
LIBS = $(shell pkg-config --libs $(PKGS)) -lm
endif

PROGRAM = $(TARGET)$(EXE_EXT)

.PHONY: all run clean check-deps

all: $(PROGRAM)

$(PROGRAM): $(SRC)
	$(CC) $(CFLAGS) $(INC_DIRS) $(SRC) -o $(PROGRAM) $(LIB_DIRS) $(LIBS)
ifeq ($(OS),Windows_NT)
	copy "$(SDL_BIN_DIR)/SDL3.dll" ".\SDL3.dll"
	copy "$(SDL_BIN_DIR)/SDL3_image.dll" ".\SDL3_image.dll"
	copy "$(SDL_BIN_DIR)/SDL3_ttf.dll" ".\SDL3_ttf.dll"
endif

run: $(PROGRAM)
	$(RUN_PREFIX)$(PROGRAM) assets/images/kodim23.png

check-deps:
ifeq ($(OS),Windows_NT)
	@echo SDL_DIR=$(SDL_DIR)
else
	pkg-config --modversion $(PKGS)
endif

clean:
ifeq ($(OS),Windows_NT)
	-del /Q $(PROGRAM) output_image.png SDL3.dll SDL3_image.dll SDL3_ttf.dll
else
	$(RM) $(PROGRAM) output_image.png
endif

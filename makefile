.DEFAULT_GOAL := all
TARGET = programa
SRC = src/main.c
PKGS = sdl3 sdl3-image sdl3-ttf
CC = gcc
CFLAGS = -std=c17 -Wall -Wextra -Wpedantic
PKG_CONFIG ?= pkg-config
IMAGE ?= assets/images/kodim23.png

ifeq ($(OS),Windows_NT)
SHELL := cmd.exe
.SHELLFLAGS := /c
EXE_EXT = .exe
RUN_PREFIX = .\
# Accept a combined SDK, the local portable packages, or the course SDK.
ifdef SDL_DIR
SDL_DIRS := $(SDL_DIR)
else
SDL_DIRS := $(wildcard .local/SDL3-*/x86_64-w64-mingw32 .local/SDL3_image-*/x86_64-w64-mingw32 .local/SDL3_ttf-*/x86_64-w64-mingw32)
ifeq ($(strip $(SDL_DIRS)),)
SDL_DIRS := $(wildcard d:/dev/compvis/libs/SDL3)
endif
endif
ifeq ($(CC),gcc)
ifneq ($(wildcard .local/w64devkit/bin/gcc.exe),)
CC := .\.local\w64devkit\bin\gcc.exe
endif
endif
endif

PROGRAM = $(TARGET)$(EXE_EXT)
ifneq ($(strip $(SDL_DIRS)),)
INC_DIRS = $(foreach dir,$(SDL_DIRS),-I"$(dir)/include")
LIB_DIRS = $(foreach dir,$(SDL_DIRS),-L"$(dir)/lib")
LIBS = -lSDL3 -lSDL3_image -lSDL3_ttf -lm
else
INC_DIRS = $(shell $(PKG_CONFIG) --cflags $(PKGS))
LIBS = $(shell $(PKG_CONFIG) --libs $(PKGS)) -lm
endif

.PHONY: all run clean check-deps runtime
all: $(PROGRAM) runtime

$(PROGRAM): $(SRC) makefile | check-deps
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INC_DIRS) $(SRC) -o $@ $(LDFLAGS) $(LIB_DIRS) $(LIBS)

check-deps:
	@$(CC) --version
ifeq ($(strip $(SDL_DIRS)),)
	@$(PKG_CONFIG) --exists $(PKGS) || (echo ERROR: Install SDL3, SDL3_image, SDL3_ttf development packages and pkg-config, or set SDL_DIR on Windows. && exit 1)
else
	@echo SDL directories: $(SDL_DIRS)
endif

# Copy all SDK DLLs, including any transitive dependencies, even on a repeat build.
runtime: $(PROGRAM)
ifeq ($(OS),Windows_NT)
ifneq ($(strip $(SDL_DIRS)),)
	$(foreach dir,$(SDL_DIRS),xcopy /D /Y /I "$(subst /,\,$(dir))\bin\*.dll" "." >nul && ) ver >nul
endif
endif

run: all
	$(RUN_PREFIX)$(PROGRAM) "$(IMAGE)"

clean:
ifeq ($(OS),Windows_NT)
	@if exist $(PROGRAM) del /Q $(PROGRAM)
else
	$(RM) $(PROGRAM)
endif

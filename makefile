config ?= debug
# config ?= release

CC := clang
CFLAGS := -Wall -Wextra -lm
DEBUG_FLAGS := -DDEBUG -g -fsanitize=address
RELEASE_FLAGS := -DNDEBUG -O3
IFLAGS := -I./vendor/include

ifeq ($(config), debug)
	CFLAGS += $(DEBUG_FLAGS)
else
	CFLAGS += $(RELEASE_FLAGS)
endif

.PHONY: build

all: build run

run:
	./build/main.bin

build:
	@-mkdir build
	$(CC) src/main.c -o build/main.bin $(IFLAGS) $(CFLAGS)


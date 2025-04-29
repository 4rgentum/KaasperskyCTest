CC = gcc
CFLAGS = -Iinclude -Wall -Wextra
SRC = src/main.c src/manifest.c src/hash.c src/sha256.c
OBJ = $(SRC:.c=.o)
BIN = mangen

# Получение Git хэша
GIT_HASH := $(shell git rev-parse --short HEAD 2>/dev/null || echo "unknown")
CFLAGS += -DGIT_HASH=\"$(GIT_HASH)\"

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)

test: $(BIN)
	chmod +x ./tests/test_end_to_end.sh
	./tests/test_end_to_end.sh

.PHONY: all clean test
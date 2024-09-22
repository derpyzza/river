CC = clang
PROGNAME = rvrcc
SRC = src
BIN = bin
TESTS = tests

EXEC = $(BIN)/$(PROGNAME)

CFLAGS := -Wall -pedantic -g
LDFLAGS :=

SRC_FILES := $(wildcard $(SRC)/*.c) $(wildcard $(SRC)/**/*.c) $(wildcard $(SRC)/**/**/*.c)
OBJ_FILES := $(patsubst $(SRC)/%.c, $(BIN)/%.o, $(SRC_FILES))

all: $(EXEC)

$(BIN)/%.o: $(SRC)/%.c 
	$(CC) $^ -c -o $@ $(CFLAGS) $(LDFLAGS)

$(EXEC): $(OBJ_FILES)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

run: $(EXEC)
	$(EXEC) in.rvr

clean:
	rm -rf $(BIN)/*

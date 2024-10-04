CC = clang
PROGNAME = rcc
SRC = src
BIN = bin
TESTS = tests

EXEC = $(BIN)/$(PROGNAME)

CFLAGS := -Wall -pedantic -Wswitch -g
LDFLAGS :=

SRC_FILES := $(wildcard $(SRC)/*.c) $(wildcard $(SRC)/**/*.c) $(wildcard $(SRC)/**/**/*.c)
OBJ_FILES := $(patsubst $(SRC)/%.c, $(BIN)/%.o, $(SRC_FILES))

all: $(EXEC)

$(BIN)/%.o: $(SRC)/%.c 
	$(CC) $^ -c -o $@ $(CFLAGS) $(LDFLAGS)

$(EXEC): $(OBJ_FILES)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

repl: $(EXEC)
	$(EXEC)

run: $(EXEC)
	$(EXEC) in.rvr

# in.rvr

clean:
	rm -rf $(BIN)/*


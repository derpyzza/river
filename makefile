CC = clang++
PROGNAME = rcc
SRC = src
BIN = bin
TESTS = tests

EXEC = $(BIN)/$(PROGNAME)

CFLAGS := -Wall -pedantic -Wswitch -g -ggdb3 
# --std=c99
LDFLAGS :=

SRC_FILES := $(wildcard $(SRC)/**.cc) $(wildcard $(SRC)/**/**.cc) $(wildcard $(SRC)/**/**/**.cc)
OBJ_FILES := $(patsubst $(SRC)/%.cc, $(BIN)/%.o, $(SRC_FILES))

all: $(EXEC)

$(BIN)/%.o: $(SRC)/%.cc 
	@mkdir -p $(dir $@)
	$(CC) $^ -c $(CFLAGS) -o $@

$(EXEC): $(OBJ_FILES)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

repl: $(EXEC)
	$(EXEC)

run: $(EXEC)
	$(EXEC) in.rvr

# in.rvr

clean:
	rm -rf $(BIN)/*

#pragma once

#include "common.h"

#define NUM_KEY_WORDS 14
static const char* key_words[NUM_KEY_WORDS] = {
	"int", 
	"bool",
	"string",
	"return",
	"if",
	"else",
	"while",
	"for",
	"struct",
	"defer",
	"namespace", 
	"print",
	"true",
	"false"
};

typedef enum token_type {
	TKN_NONE = 0,
	// Single Character tokens
	TKN_EQUAL_SIGN,
	TKN_PAREN_OPEN,
	TKN_PAREN_CLOSE,
	TKN_BRACE_OPEN,
	TKN_BRACE_CLOSE,
	TKN_OPEN_QUOTE,
	TKN_CLOSE_QUOTE,
	TKN_COMMA,
	TKN_DOT,
	TKN_PLUS,
	TKN_MINUS,
	TKN_ASTERISK,
	TKN_FORWARD_SLASH,
	TKN_SEMI,

	// keywords
	TKN_INT,
	TKN_BOOL,
	TKN_STRING,
	TKN_RETURN, 
	TKN_IF,
	TKN_ELSE,
	TKN_WHILE,
	TKN_FOR,
	TKN_STRUCT,
	TKN_DEFER,
	TKN_NAMESPACE,
	TKN_PRINT,

	TKN_VALUE,
	TKN_IDENTIFIER,
	TKN_INTEGER_LITERAL,
	TKN_FLOATING_LITERAL,
	TKN_DOUBLE_FLOATING_LITERAL,
	TKN_CHAR_LITERAL,
	TKN_STRING_LITERAL,
	TKN_TRUE,
	TKN_FALSE,

	// operators
	TKN_BANG,
	TKN_BANG_EQ,
	TKN_DIV_EQ,
	TKN_MULT_EQ,
	TKN_ADD_EQ,
	TKN_SUB_EQ,
	TKN_NOT_EQ,
	TKN_EQ_EQ,
	TKN_LESS_THAN,
	TKN_LESS_THAN_EQ,
	TKN_GREATER_THAN,
	TKN_GREATER_THAN_EQ,

	TKN_EOF,

	MAX_TKNS
} token_type;

static const char* token_strings[] = {
	"NONE",
	// Single Character tokens
	"EQUAL SIGN",
	"PAREN OPEN",
	"PAREN CLOSE",
	"BRACE OPEN",
	"BRACE CLOSE",
	"OPEN QUOTE",
	"CLOSE QUOTE",
	"COMMA",
	"DOT",
	"PLUS",
	"MINUS",
	"ASTERISK",
	"FORWARD SLASH",
	"SEMI",

	// keywords
	"INT",
	"BOOL",
	"STRING",
	"RETURN", 
	"IF",
	"ELSE",
	"WHILE",
	"FOR",
	"STRUCT",
	"DEFER",
	"NAMESPACE",
	"PRINT",

	"VALUE",
	"IDENTIFIER",
	"INTEGER LITERAL",
	"FLOATING LITERAL",
	"DOUBLE FLOATING LITERAL",
	"CHAR LITERAL",
	"STRING LITERAL",
	"TRUE",
	"FALSE",

	// operators
	"BANG",
	"BANG EQ",
	"DIV EQ",
	"MULT EQ",
	"ADD EQ",
	"SUB EQ",
	"NOT EQ",
	"EQ EQ",
	"LESS THAN",
	"LESS THAN EQ",
	"GREATER THAN",
	"GREATER THAN EQ",

	"EOF",
};

typedef enum literal_type {
	LIT_NONE = 0,
	LIT_INT,
	LIT_FLOAT,
	LIT_DOUBLE,
	LIT_CHAR,
	LIT_STRING,
	LIT_TRUE,
	LIT_FALSE,
} literal_type;

typedef struct {
	int len;
	char* c_ptr;
}substr_s;

typedef union literal_u {
	int _int;
	float _float;
	double _double;
	char _char;
	short _bool;
	substr_s _str;
} literal_u;

typedef struct literal_s {
	literal_type type;
	literal_u literal;
} literal_s;

typedef struct token_s {
	token_type type;
	int chr_index; // the starting character index
	int has_literal; // whether or not it has a literal;
	int literal_id;
} token_s;

// Dynamic array of tokens;
typedef struct token_array_s {
	long max_tokens;
	long final_token;
	long max_literals;
	long final_literal;
	token_s* token_list;
	literal_s* literal_list;
} token_array_s;

void print_token_array(string_s src, token_array_s tkn);
token_array_s* tokenize (string_s src);

// Returns an eof token. useful for termination
static inline token_s token_eof(void) {
	return (token_s) {
		.type = TKN_EOF,
		.chr_index = 0,
		.has_literal = 0,
		.literal_id = 0
	};
} 

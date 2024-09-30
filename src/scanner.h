#pragma once

#include "common.h"

#define NUM_KEY_WORDS 25
static const char* key_words[NUM_KEY_WORDS] = {

	"ubyte", "ushort", "uint", "ulong",
	"byte", "short", "int", "long",

	"float", "double",

	"void", "bool", "char", "str",

	"return",
	"if",
	"else",
	"while",
	"for",
	"struct",
	"defer",
	"namespace", 
	"true",
	"false",
	"null"
};

typedef enum token_type {
	TKN_NONE = 0,
	// Single Character tokens
	TKN_EQUAL_SIGN,
	TKN_PAREN_OPEN,
	TKN_PAREN_CLOSE,
	TKN_BRACE_OPEN,
	TKN_BRACE_CLOSE,
	TKN_QUOTE,
	TKN_COMMA, 
	TKN_DOT,
	TKN_PLUS,
	TKN_MINUS,
	TKN_ASTERISK,
	TKN_FORWARD_SLASH,
	TKN_SEMI,
	TKN_BANG,
	TKN_QUESTION,
	TKN_COLON,
	TKN_AMP,
	TKN_PIPE,
	TKN_TILDE,
	TKN_HAT,

	// keywords

	TKN_UBYTE,
	TKN_USHORT,
	TKN_UINT,
	TKN_ULONG,

	TKN_BYTE,
	TKN_SHORT,
	TKN_INT,
	TKN_LONG,

	TKN_FLOAT,
	TKN_DOUBLE,

	TKN_VOID,
	TKN_BOOL,
	TKN_CHAR,
	TKN_STR,

	TKN_RETURN, 
	TKN_IF,
	TKN_ELSE,
	TKN_WHILE,
	TKN_FOR,
	TKN_STRUCT,
	TKN_DEFER,
	TKN_NAMESPACE,
	TKN_TRUE,
	TKN_FALSE,
	TKN_NULL,

	TKN_IDENTIFIER,
	TKN_INTEGER_LITERAL,
	TKN_FLOATING_LITERAL,
	TKN_DOUBLE_FLOATING_LITERAL,
	TKN_CHAR_LITERAL,
	TKN_STRING_LITERAL,

	// operators
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
	"PAREN OPEN", "PAREN CLOSE",
	"BRACE OPEN", "BRACE CLOSE",
	"QUOTE", "COMMA", "DOT",
	"PLUS", "MINUS",
	"ASTERISK", "FORWARD SLASH",
	"SEMI", "BANG",
	"QUESTION", "COLON",
	"AMP", "PIPE",
	"TILDE", "HAT",

	// keywords
	//
	// unsigned int types
	"UBYTE","USHORT",
	"UINT","ULONG",

	// signed int types
	"BYTE","SHORT",
	"INT","LONG",

	// float types
	"FLOAT", "DOUBLE",

	// other types
	"VOID",
	"BOOL",
	"CHAR",
	"STR",

	// reserved words
	"RETURN", 
	"IF",
	"ELSE",
	"WHILE",
	"FOR",
	"STRUCT",
	"DEFER",
	"NAMESPACE",
	"TRUE",
	"FALSE",
	"NULL",

	"IDENTIFIER",
	"INTEGER LITERAL",
	"FLOATING LITERAL",
	"DOUBLE FLOATING LITERAL",
	"CHAR LITERAL",
	"STRING LITERAL",

	// operators
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

typedef struct substr_s{
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
	int has_literal;
	int literal_id;
	int line;
} token_s;

// Dynamic array of tokens;
typedef struct token_array_s {
	long max_tokens;
	long current_token;
	long max_literals;
	long current_literal;
	token_s* token_list;
	literal_s* literal_list;
} token_array_s;

void print_token_array(string_s src, token_array_s tkn);
token_array_s* tokenize (string_s src);

static inline char* token_to_str(token_type type) {
	return (char*) token_strings[type];
}

// useful for termination
static inline token_s token_eof(void) {
	return (token_s) {
		.type = TKN_EOF,
		.chr_index = 0,
		.has_literal = 0,
		.literal_id = 0
	};
} 

static inline token_s token_none(void) {
	return (token_s) {
		.type = TKN_NONE,
		.chr_index = 0,
		.has_literal = 0,
		.literal_id = 0
	};
} 

static inline char token_to_char(token_type token) {
	if (token - 1 <= TKN_BANG && token - 1 >= TKN_EQUAL_SIGN) {
		char tokens[(TKN_RETURN - TKN_EQUAL_SIGN)] = {
			'=', '(', ')', '{', '}', '\"', ',', '.', '+', '-', '*', '/', ';', '!',
		};
		return tokens[token - 1];
	}
	return 0;
}

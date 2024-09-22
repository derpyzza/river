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
	TKN_TRUE,
	TKN_FALSE,

	TKN_VALUE,
	TKN_IDENTIFIER,
	TKN_INTEGER_LITERAL,
	TKN_FLOATING_LITERAL,
	TKN_CHAR_LITERAL,
	TKN_STRING_LITERAL,

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
	"TKN_EQUAL_SIGN",
	"TKN_PAREN_OPEN",
	"TKN_PAREN_CLOSE",
	"TKN_BRACE_OPEN",
	"TKN_BRACE_CLOSE",
	"OPEN QUOTE",
	"CLOSE QUOTE",
	"TKN_COMMA",
	"TKN_DOT",
	"TKN_PLUS",
	"TKN_MINUS",
	"TKN_ASTERISK",
	"TKN_FORWARD_SLASH",
	"TKN_SEMI",

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
	"TRUE",
	"FALSE",

	"VALUE",
	"IDENTIFIER",
	"INTEGER LITERAL",
	"FLOATING LITERAL",
	"CHAR LITERAL",
	"STRING LITERAL",

	// operators
	"TKN_BANG",
	"TKN_BANG_EQ",
	"TKN_DIV_EQ",
	"TKN_MULT_EQ",
	"TKN_ADD_EQ",
	"TKN_SUB_EQ",
	"TKN_NOT_EQ",
	"TKN_EQ_EQ",
	"TKN_LESS_THAN",
	"TKN_LESS_THAN_EQ",
	"TKN_GREATER_THAN",
	"TKN_GREATER_THAN_EQ",

	"TKN_EOF",
};

typedef enum literal_type {
	LIT_NONE = 0,
	LIT_INT,
	LIT_FLOAT,
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
	char _char;
	short _bool;
	substr_s _str;
} literal_u;

typedef struct literal_s {
	literal_type type;
	literal_u literal;
	// int line, offset, len;
} literal_s;

typedef struct token_s {
	token_type type;
	int chr_index; // the starting character index
	int has_literal; // whether or not it has a literal;
	int literal_id;
} token_s;

// Dynamic array of tokens;
typedef struct ast_s {
	long max_tokens;
	long current_token;
	token_s* token_list;
	long max_literals;
	long current_literal;
	literal_s* literal_list;
} token_array_s;

// token_array_s* init_token_array(void);
// void push_token(token_array_s* tkn, token_type token, int chr_index);
// void push_token_val( token_array_s* tkn, token_type token, literal_s lit, int chr_index );
void print_token_array(file_s src, token_array_s tkn);
token_array_s* tokenize (file_s src);

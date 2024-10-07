#pragma once

#include "common.h"

typedef enum token_type {
	T_NONE = 0,
	// Single Character tokens
	T_EQUAL_SIGN,
	T_PAREN_OPEN, T_PAREN_CLOSE,
	T_BRACE_OPEN, T_BRACE_CLOSE,
	T_BRACKET_OPEN, T_BRACKET_CLOSE,
	T_COMMA, T_DOT,
	T_PLUS, T_MINUS,
	T_ASTERISK, T_FORWARD_SLASH,
	T_MODULUS, T_SEMI, T_BANG,
	T_LESS_THAN, T_GREATER_THAN,
	T_QUESTION, T_COLON,
	T_AMP, T_PIPE, 
	T_TILDE, T_HAT,

	// combo operators
	T_DIV_EQ,
	T_MULT_EQ,
	T_ADD_EQ,
	T_SUB_EQ,
	T_MOD_EQ,
	T_NOT_EQ,
	T_AND_EQ,
	T_OR_EQ,
	T_EQ_EQ,
	T_LESS_THAN_EQ,
	T_GREATER_THAN_EQ,
	T_ARROW,
	T_FAT_ARROW,
	T_DOT_DOT,
	T_LOG_AND,
	T_LOG_OR,
	T_SHL,
	T_SHR,

	T_IDENTIFIER,
	T_INTEGER_LITERAL,
	T_FLOATING_LITERAL,
	T_DOUBLE_FLOATING_LITERAL,
	T_CHAR_LITERAL,
	T_STRING_LITERAL,

	// == keywords ==

	// built in types
	// int types
	T_UBYTE, T_USHORT, T_UINT,T_ULONG,
	T_BYTE, T_SHORT, T_INT, T_LONG,
	// floats
	T_FLOAT, T_DOUBLE,
	// size
	T_USIZE, T_ISIZE,
	// alphanumeric
	T_CHAR, T_STR,
	//misc
	T_VOID, T_BOOL,


	// misc keywords
	T_RETURN, 
	T_IF, T_ELSE, T_DO, T_WHILE, T_FOR, T_IN,
	T_GOTO, T_LABEL, T_BREAK, T_CONTINUE, T_DEFER,
	T_STRUCT, T_TYPE, T_ENUM, T_UNION,
	T_USING, T_NAMESPACE, T_IMPORT, T_AS,
	T_SWITCH, T_CASE, T_SIZEOF, T_TYPEOF,
	T_MACRO, T_YIELD, T_EXPECT, T_ASSERT,
	T_STATIC, T_CONST, T_MUT, T_PUB, T_VAL,
	T_FN,

	// literals
	T_TRUE, T_FALSE, T_NULL,

	T_EOF,

	MAX_TKNS,
} token_type;

static const int NUM_KEY_WORDS = T_NULL - T_UBYTE;

static const char* token_strings[MAX_TKNS] = {
	"==NONE==",
	// Single Character tokens
	"equal sign",
	"paren open", "paren close",
	"brace open", "brace close",
	"bracket open", "bracket close",
	"comma", "dot",
	"plus", "minus",
	"asterisk", "forward slash",
	"modulus", "semi", "bang",
	"less than", "greater than",
	"question", "colon",
	"amp", "pipe",
	"tilde", "hat",

	// combo operators
	"div eq",
	"mult eq",
	"add eq",
	"sub eq",
	"mod eq",
	"not eq",
	"and eq",
	"or eq",
	"eq eq",
	"less than eq",
	"greater than eq",
	"arrow",
	"fat arrow",
	"dot dot",
	"logical and",
	"logical or",
	"shift left",
	"shift right",

	"identifier",
	"integer literal",
	"float literal",
	"double literal",
	"char literal",
	"string literal",

	// ==keywords==
	// types
	// int
	"ubyte", "ushort", "uint", "ulong",
	"byte", "short", "int", "long",
	// float
	"float", "double",
	// size
	"usize", "isize",
	// alphanumeric
	"char",  "string",
	// other
	"void", "bool", 


	// keywords
	"return", 
	"if", "else", "do", "while", "for", "in",
	"goto", "label", "break", "continue", "defer", 
	"struct", "type", "enum", "union",
	"using", "namespace", "import", "as",
	"switch", "case", "sizeof", "typeof",
	"macro", "yield", "expect", "assert",
	// variable decl
	"static", "const", "mut", "pub", "val",
	"fn",

	// literals
	"true", "false", "null",

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
	substr_s source; // the source string for this token;
	int chr_index; 	// the starting character index
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
		.type = T_EOF,
		.chr_index = 0,
		.has_literal = 0,
		.literal_id = 0
	};
} 

static inline token_s token_none(void) {
	return (token_s) {
		.type = T_NONE,
		.chr_index = 0,
		.has_literal = 0,
		.literal_id = 0
	};
} 

static inline char token_to_char(token_type token) {
	if (token <= T_HAT && token >= T_EQUAL_SIGN) {
		char tokens[(T_HAT - T_EQUAL_SIGN) + 1] = {
			'=', '(', ')', '{', '}', '[', ']', ',', '.', '+', '-', '*', '/', '%', ';', '!',
			'<', '>', '?', ':', '&', '|', '~', '^'
		};
		// the minus one is to deal with the fact that in the 
		// token_type enum T_EQUAL_SIGN is one but in tokens it's 0
		return tokens[token - 1];
	}
	return 0;
}

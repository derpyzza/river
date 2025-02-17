#pragma once

#include "utils.h"

typedef enum TokenType {
	T_NONE = 0,
	// Single Character tokens
	T_EQUAL,
	T_PAREN_OPEN, T_PAREN_CLOSE,
	T_BRACE_OPEN, T_BRACE_CLOSE,
	T_BRACKET_OPEN, T_BRACKET_CLOSE,
	T_COMMA, T_DOT,
	T_PLUS, T_MINUS,
	T_STAR, T_SLASH,
	T_MODULUS, T_SEMI, T_BANG,
	T_LESS_THAN, T_GREATER_THAN,
	T_QUESTION, T_COLON,
	T_AMP, T_PIPE, T_ATSIGN,
	T_TILDE, T_DOLLAR, T_HASH, T_HAT,
	T_SINGLE_OP_END,

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
	// T_INC, // ++
	// T_DEC, // --
	T_ARROW,
	T_FAT_ARROW,
	T_DOT_DOT,
	T_LOG_AND,
	T_LOG_OR,
	T_SHL,
	T_SHR,
	T_COMBO_OP_END,

	T_IDENTIFIER,
	T_INTEGER_LITERAL,
	T_FLOATING_LITERAL,
	T_CHAR_LITERAL,
	T_STRING_LITERAL,
	// literals
	T_TRUE, T_FALSE, T_NULL,
	T_LITERAL_END,

	// == keywords ==

	// built in types
	// int types
	T_UBYTE, T_USHORT, T_UINT, T_ULONG,
	T_BYTE, T_SHORT, T_INT, T_LONG, // uint, int ( word size integers )
	// floats
	T_FLOAT, T_DOUBLE, // f80
	// size
	T_USIZE, T_SIZE,
	// alphanumeric
	T_CHAR, T_STRING,
	//misc
	T_VOID, T_BOOL,


	// misc keywords
	T_RETURN, 
	T_IF, T_ELSE, T_THEN, T_DO, T_WHILE, T_FOR, T_IN,
	T_GOTO, T_LABEL, T_BREAK, T_CONTINUE, T_DEFER,
	T_STRUCT, T_TYPE, T_ENUM, T_UNION,
	T_IMPORT, T_AS,
	T_MATCH, T_CASE, T_SIZEOF, T_TYPEOF,
	T_MACRO, T_YIELD, T_EXPECT, T_ASSERT,
	T_FUN, T_STATIC, T_CONST, T_MUT, T_UNDEF, T_PUB, T_LET,
	T_KEYWORD_END,

	T_EOF,

	MAX_TKNS,
} TokenType;

static const int NUM_KEY_WORDS = T_EOF - T_TRUE;

static const char* token_strings[MAX_TKNS] = {
	"==NONE==",
	// Single Character tokens
	"=",
	"(", ")",
	"{", "}",
	"[", "]",
	",", ".",
	"+", "-",
	"*", "/",
	"%", ";", "!",
	"<", ">",
	"?", ":",
	"&", "|", "@",
	"~", "^",
	"sing-op-end",

	// combo operators
	"/=",
	"*=",
	"+=",
	"-=",
	"%=",
	"!=",
	"&=",
	"|=",
	"==",
	"<=",
	">=",
	// "--",
	// "++",
	"->",
	"=>",
	"..",
	"&&",
	"||",
	"<<",
	">>",
	"combo-op-end",

	"identifier",
	// literals
	"integer literal",
	"float literal",
	"char literal",
	"string literal",
	"true", "false", "null",
	"literal-end",

	// ==keywords==
	// types
	// int
	"ubyte", "ushort", "uint", "ulong",
	"byte", "short", "int", "long",
	// float
	"float", "double",
	// size
	"usize", "size",
	// alphanumeric
	"char",  "string",
	// other
	"void", "bool", 


	// keywords
	"return", 
	"if", "else", "then", "do", "while", "for", "in",
	"goto", "label", "break", "continue", "defer", 
	"struct", "type", "enum", "union",
	"import", "as",
	"switch", "case", "sizeof", "typeof",
	"macro", "yield", "expect", "assert",
	// variable decl
	"fun", "static", "const", "mut", "pub", "let",
	"keywords end",

	"EOF",
};

// category type
typedef enum TokCat {
	TC_NONE,
	TC_KEYWORD,
	TC_LITERAL,
	TC_ID,
	TC_SYMBOL,
	TC_OPERATOR,
	TC_MAX
} TokCat;

typedef struct Span {
	usize start, end;
} Span;

typedef struct Token {
	usize line;
	int chr_index; 			// the starting character index
	Span span;
	ilong ival; 				// storage for int literals
	long double fval; 	// storage for float
	TokCat cat; 				// the token category
	TokenType type; 
} Token;

// Dynamic array of tokens;
typedef struct TokenArray {
	size max;
	size current;
	Token* tokens;
} TokenArray;

void print_token_array(String *src, TokenArray tkn);
TokCat tok_to_cat( TokenType token);

// useful for termination
static inline Token token_eof(void) {
	return (Token) {
		.chr_index = 0,
		.type = T_EOF,
	};
} 

static inline Token token_none(void) {
	return (Token) {
		.chr_index = 0,
		.type = T_NONE,
	};
} 


static inline const char* token_to_str(TokenType type) {
	return (char*) token_strings[type];
}

static inline const char* tokcat_to_str(TokCat cat) {
	switch(cat) {
		case TC_KEYWORD: return "Keyword"; break;
		case TC_ID: return "Identifier"; break;
		case TC_SYMBOL: return "Symbol"; break;
		case TC_OPERATOR: return "Operator"; break;
		case TC_LITERAL: return "Literal"; break;
		default: return "tc_none"; break;
	}
}

static inline char token_to_char(TokenType token) {
	if (token <= T_HAT && token >= T_EQUAL) {
		char tokens[(T_HAT - T_EQUAL) + 1] = {
			'=', '(', ')', '{', '}', '[', ']', ',', '.', '+', '-', '*', '/', '%', ';', '!',
			'<', '>', '?', ':', '&', '|', '~', '^'
		};
		// the minus one is to deal with the fact that in the 
		// token_type enum T_EQUAL_SIGN is one but in tokens it's 0
		return tokens[token - 1];
	}
	return 0;
}

#pragma once


// Forward declarations because C is stupid about these
#include "utils.h"

typedef enum TokenTag {
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

	T_IDENTIFIER,
	T_INTEGER_LITERAL,
	T_FLOATING_LITERAL,
	T_CHAR_LITERAL,
	T_STRING_LITERAL,
	// literals
	T_TRUE, T_FALSE, T_NULL,

		// misc keywords
	T_RETURN, 
	T_IF, T_ELSE, T_THEN, T_DO, T_WHILE, T_FOR, T_IN,
	T_GOTO, T_LABEL, T_BREAK, T_CONTINUE, T_DEFER,
	T_STRUCT, T_TYPE, T_ENUM, T_UNION,
	T_IMPORT, T_AS,
	T_MATCH, T_CASE, T_SIZEOF, T_TYPEOF,
	T_MACRO, T_YIELD, T_EXPECT, T_ASSERT,
	T_FUN, T_STATIC, T_CONST, T_MUT, T_UNDEF, T_PUB, T_LET,

	T_EOF,

	MAX_TKNS,
} TokenTag;

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
	"~", "$", "#", "^",

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

	"identifier",
	// literals
	"integer literal",
	"float literal",
	"char literal",
	"string literal",
	"true", "false", "null",
	
	// keywords
	"return", 
	"if", "else", "then", "do", "while", "for", "in",
	"goto", "label", "break", "continue", "defer", 
	"struct", "type", "enum", "union",
	"import", "as",
	"switch", "case", "sizeof", "typeof",
	"macro", "yield", "expect", "assert",
	// variable decl
	"fun", "static", "const", "mut", "undef", "pub", "let",

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

typedef struct Token {
	usize line;
	int chr_index; 			// the starting character index
	String span;
	TokCat cat; 				// the token category
	TokenTag type; 
} Token;

CREATE_VEC_TYPE(Token, Token)

void print_token_array(String *src, VecToken tkn);
VecToken* tokenize( String *src );

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

static inline TokCat tok_to_cat(TokenTag token) {
	if ( IN_RANGE_INC(token, T_EQUAL, T_HAT) ) {
		return TC_SYMBOL;
	}
	if ( IN_RANGE_INC(token, T_DIV_EQ, T_SHR) ) {
		return TC_OPERATOR;
	}
	if ( IN_RANGE_INC(token, T_INTEGER_LITERAL, T_NULL) )  {
		return TC_LITERAL;
	}
	if ( IN_RANGE_INC(token, T_RETURN, T_LET) ) {
		return TC_KEYWORD;
	}	
	if ( token == T_IDENTIFIER ) return TC_ID;
	return TC_NONE;
}

static inline const char* tok_to_str(TokenTag type) {
	return (char*) token_strings[type];
}

static inline const char* cat_to_str(TokCat cat) {
	switch(cat) {
		case TC_KEYWORD: return "Keyword"; break;
		case TC_ID: return "Identifier"; break;
		case TC_SYMBOL: return "Symbol"; break;
		case TC_OPERATOR: return "Operator"; break;
		case TC_LITERAL: return "Literal"; break;
		default: return "tc_none"; break;
	}
}

static inline char tok_to_char(TokenTag token) {
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

#pragma once


// Forward declarations because C is stupid about these
#include <libderp/derp.h>
#include <stdlib.h>

#include <stdio.h>

// hacky yet convenient macro for defining token enums along with their debug print strings
// tokens with a double underscore prefix like __T_[TOKEN] are reserved tokens that
// may or may not get added as keywords later down the line.
#define TKNS                                    \
	\
	TKN(T_ASSIGN,                  ":=")          \
	\
  TKN(T_DIV_EQ,                  "/=")          \
  TKN(T_MUL_EQ,                  "*=")          \
  TKN(T_ADD_EQ,                  "+=")          \
  TKN(T_SUB_EQ,                  "-=")          \
  TKN(T_MOD_EQ,                  "%=")          \
  \
  TKN(T_BAND_EQ,                 "&=")          \
  TKN(T_BOR_EQ,                  "|=")          \
  TKN(T_BIXOR_EQ,                "^=")          \
  \
  TKN(T_NOT_EQ,                  "!=")          \
  TKN(T_EQ_EQ,                   "==")          \
  TKN(T_LT_EQ,                   "<=")          \
  TKN(T_GT_EQ,                   ">=")          \
  TKN(T_SHL_EQ,                 "<<=")          \
  TKN(T_SHR_EQ,                 ">>=")          \
	\
  TKN(T_ARROW,                   "->")          \
  TKN(T_FAT_ARROW,               "=>")          \
  TKN(T_SHL,                     "<<")          \
  TKN(T_SHR,                     ">>")          \
  TKN(T_DOT_DOT,                 "..")          \
  TKN(T_ELLIPSE,                "...")          \
  TKN(T_LAND,                    "&&")          \
  TKN(T_LOR,                     "||")          \
  \
  TKN(T_LIT_INT,                 "int_lit")     \
  TKN(T_LIT_STR,                 "str_lit")     \
  TKN(T_LIT_CHAR,                "char_lit")    \
  TKN(T_LIT_FLOAT,               "float_lit")   \
  TKN(T_IDEN,                    "identifier")  \
  \
  TKN(T_KEYWORD_START,           "kwrd_start")  \
  \
  TKN(T_ALIAS,                   "alias")       \
  TKN(T_ANY,                     "any")         \
  TKN(T_AS,                      "as")          \
  TKN(T_BREAK,                   "break")       \
  TKN(T_CASE,                    "case")        \
  TKN(T_CONST,                   "const")       \
  TKN(T_CONTINUE,                "continue")    \
  TKN(T_DEFAULT,                 "default")     \
  TKN(T_DEFER,                   "defer")       \
  TKN(T_ELSE,                    "else")        \
  TKN(T_ENUM,                    "enum")        \
  TKN(T_FALSE,                   "false")       \
  TKN(T_FOR,                     "for")         \
  TKN(T_FUN,                     "fun")         \
  TKN(T_GOTO,                    "goto")        \
  TKN(T_IF,                      "if")          \
  TKN(T_IMPORT,                  "import")      \
  TKN(__T_IMPL,                  "impl")        \
  TKN(T_IN,                      "in")          \
  TKN(T_LET,                     "let")         \
  TKN(__T_MACRO,                 "macro")       \
  TKN(T_MODULE,                  "module")      \
  TKN(T_NULL,                    "null")        \
  TKN(T_PUB,                     "pub")         \
  TKN(T_RETURN,                  "return")      \
  TKN(T_STATIC,                  "static")      \
  TKN(T_STRUCT,                  "struct")      \
  TKN(T_SWITCH,                  "switch")      \
  TKN(T_TRUE,                    "true")        \
  TKN(T_TYPE,                    "type")        \
  TKN(T_UNDEF,                   "undef")       \
  TKN(T_UNION,                   "union")       \
  TKN(T_VAR,                     "var")         \
  TKN(T_WHILE,                   "while")       \
  \
	TKN(T_KEYWORD_END,             "kwrd end")    \
	TKN(T_ERROR,                   "TOKEN ERROR") \
  \
  TKN(T_EOF,                     "eof")

typedef enum TokenTag {
	T_NONE = 0,

	// Single Character tokens, ascii codes

	TKNS_START = 128,
	#define TKN(t, s) t,
		TKNS
	#undef TKN
	TKNS_MAX,
} TokenTag;

static const int NUM_KEY_WORDS = T_KEYWORD_END - T_KEYWORD_START;

// category type
typedef enum TokCat {
	TC_NONE,
	TC_KEYWORD,
	TC_LITERAL,
	TC_IDEN,
	TC_SYMBOL,
	TC_OPERATOR,
	TC_EOF,
	TC_MAX
} TokCat;

typedef struct Token {
	TokenTag type; 
	usize line;
	int chr_index; // the starting character index
	dstr span;
	TokCat cat; 	 // the token category
} Token;

const char* tok_to_str(int t);

void scanner_init(dstr src);
Token next_token(void);

// useful for termination
#define TOKEN_EOF (Token) { .chr_index = 0, .type = T_EOF, }
#define TOKEN_NONE (Token) { .chr_index = 0, .type = T_NONE, }

static inline TokCat tok_to_cat(TokenTag token) {
	if ( in_range_inc(token, '=', '^') ) {
		return TC_SYMBOL;
	}
	if ( in_range_inc(token, T_DIV_EQ, T_SHR) ) {
		return TC_OPERATOR;
	}
	if ( in_range_inc(token, T_LIT_INT, T_NULL) )  {
		return TC_LITERAL;
	}
	if ( in_range_inc(token, T_RETURN, T_LET) ) {
		return TC_KEYWORD;
	}	
	if ( token == T_IDEN ) return TC_IDEN;
	return TC_NONE;
}

static inline const char* cat_to_str(TokCat cat) {
	switch(cat) {
		case TC_KEYWORD: return "Keyword"; break;
		case TC_IDEN: return "Identifier"; break;
		case TC_SYMBOL: return "Symbol"; break;
		case TC_OPERATOR: return "Operator"; break;
		case TC_LITERAL: return "Literal"; break;
		default: return "tc_none"; break;
	}
}

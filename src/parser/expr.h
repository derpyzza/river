#pragma once

#include "parser.h"

typedef enum BinOpType {
	BIN_NONE = 0,

	BIN_ADD,  // +
	BIN_SUB,  // -
	BIN_MULT, // *
	BIN_DIV,  // /
	BIN_MOD,  // %
	// BIN_FMOD, // %%

	BIN_LT,  // <
	BIN_GT,  // >
	BIN_SHL, // <<
	BIN_SHR, // >>

	BIN_EQ, // ==
	BIN_NEQ, // !=

	BIN_ADD_EQ,  // +=
	BIN_SUB_EQ,  // -=
	BIN_MULT_EQ, // *=
	BIN_DIV_EQ,  // /=
	BIN_MOD_EQ,  // %=
	// BIN_FMOD_EQ,  // %%=
	BIN_LT_EQ,   // <=
	BIN_GT_EQ,   // >=
	BIN_SHL_EQ,  // <<=
	BIN_SHR_EQ,  // >>=

	BIN_BAND,   // &
	BIN_BOR,    // |
	BIN_BXOR,   // ^
	BIN_BNOT,   // ~

	BIN_BAND_EQ,   // &=
	BIN_BOR_EQ,    // |=
	BIN_BXOR_EQ,   // ^=

	BIN_LAND,   // and
	BIN_LOR,    // or
	BIN_LNOT,   // not

	BIN_ASSIGN,    // :=
	BIN_FUNC_APPL, // ->
	BIN_DOT,       // .
	BIN_CAST,      // as
	BIN_ELSE,      // else
} BinOpType;

typedef enum UnOpType {
	UN_NONE = 0,
	UN_MINUS,
	UN_BANG,
	UN_DEREF,
	UN_ADDR
}UnOpType;

Node* parse_expr(void);
void print_expr(Node *expr);

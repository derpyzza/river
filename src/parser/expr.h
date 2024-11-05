#pragma once

#include "parser.h"

typedef enum BinOpType {
	BIN_NONE = 0,
	BIN_ADD,
	BIN_SUB,
	BIN_MULT,
	BIN_DIV,
	BIN_MOD,
	BIN_BAND,
	BIN_BOR,
	BIN_BXOR,
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

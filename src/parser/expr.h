#pragma once

typedef struct NodeBinExpr {
	struct NodeExpr *lhs, *rhs;
	int op_type;
} NodeBinExpr;

typedef struct NodeUnExpr {
	struct NodeExpr * operand;
	enum {
		UN_NONE = 0,
		UN_MINUS,
		UN_BANG,
		UN_DEREF,
		UN_ADDR
	} operator;
}NodeUnExpr;

typedef struct NodeIfExpr {
	struct NodeExpr *cond;
	struct NodeBlock *block;
	int has_else;
	struct NodeExpr *_else;
} NodeIfExpr;

typedef struct NodeExpr {
	enum {
		E_BIN_EXPR,
		E_UN_EXPR,
		E_ARRAY,
		E_STRUCT,
		E_TUPLE,
		E_IF,
		E_WHILE,
		E_FOR,
		E_MATCH,
		E_ASSIGN,
		// i.e +=, -= &= etc.
		E_ASSIGN_OP,
		E_LIT,
		E_NULL,
	} type;
	union {
		struct NodeBinExpr *bin;
		struct NodeUnExpr *un;
		int lit;
	} expr;
}NodeExpr;

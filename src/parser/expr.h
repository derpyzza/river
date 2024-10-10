#pragma once

typedef struct NodeBinExpr {
	enum BinOpType {
		BIN_NONE = 0,
		BIN_ADD,
		BIN_SUB,
		BIN_MULT,
		BIN_DIV,
		BIN_MOD,
		BIN_BAND,
		BIN_BOR,
		BIN_BXOR,
	} op_type;
	struct NodeExpr *lhs, *rhs;
} NodeBinExpr;

typedef struct NodeUnExpr {
	struct NodeExpr * operand;
	enum UnOpType {
		UN_NONE = 0,
		UN_MINUS,
		UN_BANG,
		UN_DEREF,
		UN_ADDR
	} op_type;
}NodeUnExpr;

typedef struct NodeBlockExpr {

} NodeBlockExpr;

// if <cond> ( then <expr> ; | <blockExpr> ) (else (<expr> ;| <blockExpr>))?;
//
// <cond> -> <expr> except <block>, <
typedef struct NodeIfExpr {
	struct NodeExpr *cond;
	struct NodeBlock *block;
	int has_else;
	struct NodeExpr *_else;
} NodeIfExpr;

typedef struct NodeExpr {
	enum expr_type {
		E_NULL,
		E_BIN_EXPR,
		E_UN_EXPR,

		// E_IF,
		// E_WHILE,
		// E_FOR,
		// E_MATCH,
		// E_ASSIGN,
		// E_BLOCK,
		// i.e +=, -= &= etc.
		// E_ARRAY,
		// E_STRUCT,
		// E_TUPLE,
		//
		// E_ASSIGN_OP,
		E_LIT,
	} type;

	union {
		NodeBinExpr *bin;
		NodeUnExpr *un;
		int lit;
	};
}NodeExpr;

// struct NodeBlockExpr* block_expr(void);
struct node* parse_expr(void);
void print_expr(struct node *expr);

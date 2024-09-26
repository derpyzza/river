#pragma once

#include "scanner.h"
typedef enum {
	N_NONE,
	N_LIT_INT,
	N_EXP_UN,
	N_EXP_BIN,
} node_type;

union expr{
	struct binary {
		struct node_s *lhs, *rhs;
		int operator_type;
	} binary;
	struct unary {
		struct node_s *operand;
		int operator_type;
	} unary;
};

// AST Node 
typedef struct node_s {
	int type;

	union {
		int int_lit;
		union expr expr;
	} node;
} node_s;

void print_ast(node_s tree);
node_s parse_tokens( struct token_array_s tokens, struct string_s src ); 

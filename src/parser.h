#pragma once

#include "scanner.h"
typedef enum {
	N_UNKNOWN = -1,
	N_NONE = 0,
	N_EXP_UN,
	N_EXP_BIN,

	// literals:
	N_LIT_INT,
	N_LIT_DOUBLE,
	N_LIT_TRUE,
	N_LIT_FALSE,
	N_LIT_STRING,
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
		double double_lit;
		substr_s str_lit;
		union expr expr;
	} node;
} node_s;

void print_ast(node_s node);
node_s *parse_tokens( struct token_array_s *tokens, struct string_s src ); 

#pragma once

#include "scanner.h"
#include <stdio.h>
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

	N_FN_CALL,
	N_FN_DEF,

	N_STMT,
	N_RETURN,
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

struct func_sig {
	substr_s func_name;
	token_type return_type;
	// void *params;
};

// AST Node 
typedef struct node_s {
	int type;

	union {
		int int_lit;
		double double_lit;
		substr_s str_lit;
		union expr expr;
		struct func_def {
			struct func_sig func_sig;
			struct node_s* body;
		} func_def;
		struct func_sig func_call;
	} node;
} node_s;

void print_ast(node_s node);
node_s *parse_tokens( struct token_array_s *tokens, struct string_s src ); 
char* node_to_string(char* string, node_s node);

#pragma once

#include "scanner.h"
#include <stdio.h>
typedef enum {
	N_ERROR = -1,
	N_NONE = 0,
	N_EXP_UN,
	N_EXP_BIN,

	// literals:
	N_LIT_INT,
	N_LIT_DOUBLE,
	N_LIT_TRUE,
	N_LIT_FALSE,
	N_LIT_STRING,

	N_BLOCK,

	N_FN_CALL,
	N_FN_DEF,

	N_STMT,
	N_RETURN,
} node_type;

union expr{
	struct binary {
		struct node *lhs, *rhs;
		int operator_type;
	} binary;
	struct unary {
		struct node *operand;
		int operator_type;
	} unary;
};

struct var {
	token_type type;
	substr_s id;
};

struct func_sig {
	substr_s func_name;
	token_type return_type;
	int num_params;
	struct var* params;
};

union Stmt {
	union expr expr;
};

// modifier flags:
// 0001 0000 = public
// 0000 0001 = static
// 0000 0010 = mutable
// 0000 0100 = const

// AST Node 
typedef struct node {
	int type;
	int has_error;
	struct error {
		token_s token;
		token_type expected;
	} error;

	union {
		int int_lit;
		double double_lit;
		substr_s str_lit;
		struct block {
			int num_children;
			struct node** children;
		} block;
		union expr expr;
		struct func_def {
			struct func_sig func_sig;
			struct node *body;
			char modifiers;
		} func_def;
		struct func_sig func_call;
		struct val {
			token_type type;
			literal_s value;	
			char modifiers;
		} val;
	} node;
} node_s;

void print_ast(node_s node);
node_s *parse_tokens( struct token_array_s *tokens, struct string_s src ); 
char* node_to_string(char* string, node_s node);

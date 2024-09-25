#pragma once

typedef enum {
	N_NONE,
	N_INT_LIT,
	N_B_EXPR,
} node_type;

typedef struct bin_expr {
	int lhs, rhs;
	char op;
} bin_expr_s;

typedef struct node_s {
	// struct node_s *next;
	node_type type;
	union {
		int int_lit;
		bin_expr_s bexpr;
	};
} node_s;

void print_ast(node_s tree);
node_s parse_tokens( struct token_array_s tokens, struct string_s src ); 

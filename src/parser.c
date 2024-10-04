#include <stdio.h>

#include "scanner.h"
#include "parser.h"
#include "parser_utils.h"

// Forward declarations so that i can rearrange the definitions as i wish
static char* node_to_str(node_s node);

static node_s *new_node(node_type type);
static node_s *node_unknown(void);
static node_s *new_bin_expr(node_s *lhs, node_s *rhs, int op);
static node_s *new_unary_expr(node_s *right, int op);

static node_s *expr(void);
static node_s *term(void);
static node_s *factor(void);
static node_s *unary(void);
static node_s *primary(void);
static node_s *block(void);
static node_s *global_variable(void);
static node_s *program(void);
static node_s *function(void);
static node_s *statement(void);

node_s *parse_tokens( token_array_s *tokens, string_s src ) 
{ 
	init_parser(tokens);

	node_s *node = program();

	return node;
}
 
static node_s *program(void) {

	node_s* node = new_node(N_BLOCK);
	node->node.block.num_children = 0;
	node->node.block.children = (node_s**)malloc(sizeof(node_s*) * 8);

	while ( !expect(T_EOF) && match_expect(T_INT) && match_expect(T_IDENTIFIER)) {
		if (expect(T_PAREN_OPEN) || expect(T_FAT_ARROW)) {
			node->node.block.children[
				node->node.block.num_children
			] = function();		
			node->node.block.num_children++;
		} else if (expect(T_EQUAL_SIGN)) {
			// node->node.block.children[
			// 	node->node.block.num_children
			// ] = function();		
			// node->node.block.num_children++;
		}
	}
	return node;
}

static node_s *block(void) {
	node_s* node = new_node(N_BLOCK);
	node->node.block.num_children = 0;
	node->node.block.children = (node_s**)malloc(sizeof(node_s*) * 8);

	if(match_expect(T_BRACE_OPEN)) {
		while(!match(T_BRACE_CLOSE)) {
			node->node.block.children[
				node->node.block.num_children
			] = expr();		
			node->node.block.num_children++;
		}
		return node;
	} else {
		return new_node(N_ERROR);
	}
}

static node_s *function(void) {
	struct func_sig sig = {
		.func_name = lit_at(current()).literal._str,
		.return_type = T_INT,
		.num_params = 0,
		.params = NULL
	};
	node_s* node = new_node(N_FN_DEF);
	node->node.func_def.func_sig = sig;

	if (match(T_PAREN_OPEN)) 
	{
		while(!match(T_PAREN_CLOSE)) {
			match(T_INT);
			match(T_IDENTIFIER);
		}
	}
	if (match_expect(T_FAT_ARROW))
	{
		if (expect(T_BRACE_OPEN)) {
			node->node.func_def.body = block();
		}

		else {
			node->node.func_def.body = expr();
			match_expect(T_SEMI);
		}
	}
	return node;
}

static node_s *global_variable(void) {
	// int x;
	// int z = 0;
	// int h, y = 0;
	// int q, w, e, r;
	// int t, u = 3, i, o;
	// int a, s, d = 23;
	// int (h, j, k, l) = (1, 2, 3, 4);
	if(
			match_range(T_UBYTE, T_BOOL) ||
			match(T_IDENTIFIER)
			) {
		// type matched
		if(match(T_IDENTIFIER)) {
			// '=' which should check for a value;
			if (match(T_EQUAL_SIGN)) {
				if (match_range(T_INTEGER_LITERAL, T_STRING_LITERAL)){
					if (match(T_SEMI)) {
						// WE ARE DONE TOT
					}
				}
			}

			if(match(T_SEMI)) {
				// error: provide a definition for the global variable
				//end
			}

		}
		else { 
			//error 
		}
	} else {
		// error
	}
	return node_unknown();
}

static node_s *expr(void) {

	node_s *expr = term();

	while (match(T_PLUS) || match(T_MINUS)) {
		int op = current_tok().type;
		// printf("expr: %s, %c, %i\n", token_to_str(op), token_to_char(op), op);
		node_s *rhs = term();
		expr = new_bin_expr(expr, rhs, op);
	}
	return expr;
}

static node_s *term(void) {
	node_s *expr = factor();

	while (match(T_ASTERISK) || match(T_FORWARD_SLASH)) {
		int op = current_tok().type;
		// printf("term: %s, %c\n", token_to_str(op), token_to_char(op));
		node_s *rhs = factor();
		expr = new_bin_expr(expr, rhs, op);
	}
	return expr;
}

static node_s *factor(void) {

	if (match(T_BANG) || match(T_MINUS)) {
		int op = current_tok().type;
		node_s *right = factor();
		node_s *expr = new_unary_expr(right, op);
		return expr;
	}
	return primary();
}

static node_s *primary(void) {
	if (match(T_INTEGER_LITERAL)){
		node_s *node = new_node(N_LIT_INT);
		node->node.int_lit = lit_at(current()).literal._int;
		return node;
	}

	if (match(T_DOUBLE_FLOATING_LITERAL)){
		node_s *node = new_node(N_LIT_DOUBLE);
		node->node.double_lit = lit_at(current()).literal._double;
		return node;
	}

	if (match(T_STRING_LITERAL)){
		node_s *node = new_node(N_LIT_STRING);
		node->node.str_lit = lit_at(current()).literal._str;
		return node;
	}

	if (match(T_TRUE)){
		node_s *node = new_node(N_LIT_TRUE);
		return node;
	}

	if (match(T_FALSE)){
		node_s *node = new_node(N_LIT_FALSE);
		return node;
	}


	return node_unknown();
}

static node_s *new_node(node_type type) {
	node_s* node = malloc(sizeof(node_s));
	if (node == NULL) {
		printf("ERROR: couldn't malloc a new node\n");
		return NULL;
	}
	node->type = type;
	return node;
}

static node_s *node_unknown(void) {
	return new_node(N_UNKNOWN);
}

static node_s *new_bin_expr(node_s *lhs, node_s *rhs, int op) {
	node_s *node = new_node(N_EXP_BIN);
	node->node.expr.binary = (struct binary) {
		.lhs = lhs,
		.rhs = rhs,
		.operator_type = op
	};
	return node;
}

static node_s *new_unary_expr(node_s *right, int op) {
	node_s *node = new_node(N_EXP_UN);
	node->node.expr.unary = (struct unary) {
		.operator_type = op,
		.operand = right
	};
	return node;
}

char* node_to_string(char* string, node_s node) {
	switch(node.type) {
		case N_LIT_INT:
		{
			sprintf(string, "%i", node.node.int_lit);
			return string;
		}
		break;
		case N_EXP_BIN:
		{
			struct binary bin = node.node.expr.binary;
			sprintf(string, "%s %c %s", 
					node_to_string((char*)malloc(sizeof(char) * 64), *bin.lhs), 
					token_to_char(bin.operator_type), 
					node_to_string((char*)malloc(sizeof(char) * 64), *bin.rhs)
					);
			return string;
		}
		break;
	}
	return string;
}

void print_ast(node_s node)
{
	// printf("\n(\n");
	switch (node.type) {
		case N_NONE:
			printf("none");
		break;
		case N_UNKNOWN:
			printf("unknown node");
		break;
		case N_BLOCK: {
			int count = node.node.block.num_children;
			printf("block, children: %i\n", count);
			for (int i = 0; i < count; i++ ) {
				print_ast(*node.node.block.children[i]);
			}
			break;
		}
		case N_FN_DEF: {
			struct func_sig sig = node.node.func_def.func_sig;
			printf("\nfn: %.*s, ret: %s, body: \n\t", 
					sig.func_name.len,
					sig.func_name.c_ptr,
					token_to_str(sig.return_type)
					);
			print_ast(*node.node.func_def.body);
		}
		break;
		case N_LIT_INT:
			printf("(int %i)", node.node.int_lit);
			// return;
		break;
		case N_EXP_BIN:
		{
			printf("(bexpr ");	
			print_ast(*node.node.expr.binary.lhs);
			char c = token_to_char( node.node.expr.binary.operator_type );
			printf(" %c ", c);
			print_ast(*node.node.expr.binary.rhs);
			printf(" )");
			// return;
		}
		break;
		case N_EXP_UN:
		{
			char c = token_to_char( node.node.expr.unary.operator_type );
			printf("(unexpr %c)", c);
			print_ast(*node.node.expr.unary.operand);
		}
		break;
		default:
			// return "tree";
		break;
	}
}

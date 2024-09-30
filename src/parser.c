#include <stdio.h>

#include "scanner.h"
#include "parser.h"

struct parser {
	token_array_s *tokens;
	long current;
};

static struct parser parser;

// Forward declarations so that i can rearrange the definitions as i wish

static inline literal_s lit_at(long tok_id);
static inline token_s peek(void);
static inline token_s prev(void);
static inline token_s peek_n(int offset);
static inline token_s prev_n(int offset);
static inline token_s current(void);
static inline token_s token_at(long id);
static inline node_s node_none(void);
static inline void error_unexpected(token_type expected);
static inline void consume(void);
static inline int expect(token_type type);
static inline int match(token_type type);
static inline int match_expect(token_type expected);
static inline int expect_n(int offset, token_type expected);
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
static node_s *program(void);
static node_s *function(void);
static node_s *statement(void);

node_s *parse_tokens( token_array_s *tokens, string_s src ) 
{ 
	parser.tokens = tokens;
	parser.current = -1;

	node_s *node = program();

	return node;
}

static node_s *program(void) {

	if (match_expect(TKN_INT) && match_expect(TKN_IDENTIFIER)) {
		return function();		
	}

	return node_unknown();
}

static node_s *function(void) {
	struct func_sig sig = {
		.func_name = lit_at(parser.current).literal._str,
		.return_type = TKN_INT,
	};
	match_expect(TKN_PAREN_OPEN);
	match_expect(TKN_PAREN_CLOSE);
	match_expect(TKN_BRACE_OPEN);
	node_s* fn_node = new_node(N_FN_DEF);
	fn_node->node.func_def.func_sig = sig;
	fn_node->node.func_def.body = expr();
	match_expect(TKN_BRACE_CLOSE);
	return fn_node;
}

static node_s *expr(void) {
	node_s *expr = term();

	while (match(TKN_PLUS) || match(TKN_MINUS)) {
		int op = current().type;
		// printf("expr: %s, %c, %i\n", token_to_str(op), token_to_char(op), op);
		node_s *rhs = term();
		expr = new_bin_expr(expr, rhs, op);
	}
	return expr;
}

static node_s *term(void) {
	node_s *expr = factor();

	while (match(TKN_ASTERISK) || match(TKN_FORWARD_SLASH)) {
		int op = current().type;
		// printf("term: %s, %c\n", token_to_str(op), token_to_char(op));
		node_s *rhs = factor();
		expr = new_bin_expr(expr, rhs, op);
	}
	return expr;
}

static node_s *factor(void) {

	if (match(TKN_BANG) || match(TKN_MINUS)) {
		int op = current().type;
		node_s *right = factor();
		node_s *expr = new_unary_expr(right, op);
		return expr;
	}
	return primary();
}

static node_s *primary(void) {
	if (match(TKN_INTEGER_LITERAL)){
		node_s *node = new_node(N_LIT_INT);
		node->node.int_lit = lit_at(parser.current).literal._int;
		return node;
	}

	if (match(TKN_DOUBLE_FLOATING_LITERAL)){
		node_s *node = new_node(N_LIT_DOUBLE);
		node->node.double_lit = lit_at(parser.current).literal._double;
		return node;
	}

	if (match(TKN_STRING_LITERAL)){
		node_s *node = new_node(N_LIT_STRING);
		node->node.str_lit = lit_at(parser.current).literal._str;
		return node;
	}

	if (match(TKN_TRUE)){
		node_s *node = new_node(N_LIT_TRUE);
		return node;
	}

	if (match(TKN_FALSE)){
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

static inline void consume(void) {
	parser.current++;
}

static inline token_s peek(void) {
	return peek_n(1);
}

static inline token_s peek_n(int offset) {
	if (parser.current + offset >= parser.tokens->current_token ) {
		return token_eof();
	}
	return parser.tokens->token_list[parser.current + offset];
}

// opposite of peek
static inline token_s prev(void) {
	return prev_n(1);
}

static inline token_s prev_n(int offset)
{
	if (parser.current + offset < 0 ) {
		return token_none();
	}
	return parser.tokens->token_list[parser.current - offset];
}

static inline int expect(token_type expected)
{
	return expect_n(1, expected);
}

static inline int expect_n(int offset, token_type expected)
{
	return (peek_n(offset).type == expected);
}

static inline int match(token_type expected) {
	if (expect(expected)) {
		consume();
		return 1;
	}
	return 0;
}

static inline int match_expect(token_type expected)
{
	if (expect(expected)) {
		consume();
		return 1;
	}
	error_unexpected(expected);
	return 0;
}



static inline token_s current(void) {
	return parser.tokens->token_list[parser.current];
}

static inline void error_unexpected( token_type expected ) {
	printf(
			"ERROR: Unexpected token %s at (%i:%i), expected %s\n"
			, token_to_str(peek().type)
			, current().line
			, current().chr_index
			, token_to_str(expected)
			);
}

static inline token_s token_at(long id)
{
	if (id >= parser.tokens->current_token ) {
		return token_eof();
	}
	return parser.tokens->token_list[id];
}

static inline literal_s lit_at(long tok_id)
{
	if (tok_id >= parser.tokens->current_token) {
		printf("ERROR: tok_id exceeds total number of tokens\n");
		return (literal_s){.type=LIT_NONE};
	}
	int lit_id = token_at(tok_id).literal_id;	
	return parser.tokens->literal_list[lit_id];
}

static inline node_s node_none(void) {
	return (node_s){.type = N_NONE};
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
		case N_FN_DEF: {
			struct func_sig sig = node.node.func_def.func_sig;
			printf("fn: %.*s, ret: %s, body: \n", 
					sig.func_name.len,
					sig.func_name.c_ptr,
					token_to_str(sig.return_type)
					);
			print_ast(*node.node.func_def.body);
		}
		break;
		case N_LIT_INT:
			printf(" %i", node.node.int_lit);
			// return;
		break;
		case N_EXP_BIN:
		{
			printf(" (");	
			print_ast(*node.node.expr.binary.lhs);
			char c = token_to_char( node.node.expr.binary.operator_type );
			printf(" %c", c);
			print_ast(*node.node.expr.binary.rhs);
			printf(" )");
			// return;
		}
		break;
		case N_EXP_UN:
		{
			char c = token_to_char( node.node.expr.unary.operator_type );
			printf(" %c", c);
			print_ast(*node.node.expr.unary.operand);
		}
		break;
		default:
			// return "tree";
		break;
	}
}

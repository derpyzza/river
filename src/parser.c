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
static inline token_s 	peek(void);
static inline token_s 	prev(void);
static inline token_s 	peek_n(int offset);
static inline token_s 	prev_n(int offset);
static inline token_s 	current(void);
static inline token_s 	token_at(long id);
static inline node_s 		node_none(void);
static inline void 			error_unexpected(token_type expected);
static inline void 			consume(void);
static inline int 			expect(token_type type);
static inline int 			match(token_type type);
static inline int 			expect_n(int offset, token_type expected);
static char* 						node_to_str(node_s node);

static node_s *new_node(void);
static node_s *new_bin_expr(node_s *lhs, node_s *rhs, int op);
static node_s *new_unary_expr(node_s *right, int op);

static node_s *parse_expr(void);
static node_s *parse_term(void);
static node_s *parse_factor(void);
static node_s *parse_unary(void);
static node_s *parse_primary(void);
static node_s parse_program(void);

node_s *parse_tokens( token_array_s *tokens, string_s src ) 
{ 
	parser.tokens = tokens;
	parser.current = -1;

	node_s *node = malloc(sizeof(node_s));
	node = parse_expr();

	return node;
}

static node_s *parse_expr(void) {
	node_s *expr = parse_term();

	while (match(TKN_PLUS) || match(TKN_MINUS)) {
		int op = current().type;
		// printf("expr: %s, %c, %i\n", token_to_str(op), token_to_char(op), op);
		node_s *rhs = parse_term();
		expr = new_bin_expr(expr, rhs, op);
	}
	return expr;
}

static node_s *parse_term(void) {
	node_s *expr = parse_factor();

	while (match(TKN_ASTERISK) || match(TKN_FORWARD_SLASH)) {
		int op = current().type;
		// printf("term: %s, %c\n", token_to_str(op), token_to_char(op));
		node_s *rhs = parse_factor();
		expr = new_bin_expr(expr, rhs, op);
	}
	return expr;
}

static node_s *parse_factor(void) {

	if (match(TKN_BANG) || match(TKN_MINUS)) {
		int op = current().type;
		node_s *right = parse_factor();
		node_s *expr = new_unary_expr(right, op);
		return expr;
	}
	return parse_primary();
}

static node_s *parse_primary(void) {
	if (match(TKN_INTEGER_LITERAL)){
		node_s *node = new_node();
		node->type = N_LIT_INT;
		node->node.int_lit = lit_at(parser.current).literal._int; // current == 2
		return node;
	}
	return NULL;
}

static node_s *new_node(void) {
	node_s* node = malloc(sizeof(node_s));
	if (node == NULL) {
		printf("ERROR: couldn't malloc a new node\n");
		return NULL;
	}
	return node;
}

static node_s *new_bin_expr(node_s *lhs, node_s *rhs, int op) {
	node_s *node = new_node();
	node->type = N_EXP_BIN;
	node->node.expr.binary = (struct binary) {
		.lhs = lhs,
		.rhs = rhs,
		.operator_type = op
	};
	return node;
}

static node_s *new_unary_expr(node_s *right, int op) {
	node_s *node = new_node();
	node->type = N_EXP_UN;
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

static inline token_s current(void) {
	return parser.tokens->token_list[parser.current];
}

static inline void error_unexpected( token_type expected ) {
	printf(
			"ERROR: Unexpected token %s at (%li:%i), expected %s\n"
			, token_to_str(peek().type)
			, (long) 0
			, peek().chr_index
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

void print_ast(node_s node)
{
	// printf("\n(\n");
	switch (node.type) {
		case N_NONE:
			printf("none");
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

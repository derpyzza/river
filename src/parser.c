#include <stdio.h>

#include "scanner.h"
#include "parser.h"

struct parser {
	token_array_s *tokens;
	long current;
};

static struct parser parser;

// Forward declarations so that i can rearrange the definitions as i wish
// Helper functions:
// Returns the next token in the list;
static inline token_s peek(void);
// Returns the next token in the list as determined by the offset
static inline token_s ppeek(int offset);
// Returns the token at the given id
static inline token_s token_at(long id);
// Returns the literal at the given token id
static inline literal_s lit_at(long tok_id);

static node_s parse_expr(void);
static char* node_to_str(node_s node);

node_s parse_tokens( token_array_s tokens, string_s src ) 
{ 
	parser.tokens = &tokens;
	parser.current = 0;
	token_s cur_t = tokens.token_list[parser.current];
	switch (cur_t.type) {
		case TKN_INTEGER_LITERAL:
			if (peek().type == TKN_PLUS && ppeek(2).type == TKN_INTEGER_LITERAL && ppeek(3).type == TKN_EOF){
				return parse_expr();
			} else {
				if (peek().type == TKN_EOF) {
					return (node_s){
						.type = N_INT_LIT,
						.int_lit = lit_at(parser.current).literal._int
					};
				}
			}
		break;
		default:
		break;
	}
	return (node_s){0};
}

static node_s parse_expr (void)
{
	node_s node = (node_s){
		.type = N_B_EXPR, 
		.bexpr = (bin_expr_s){
			.lhs = lit_at(parser.current).literal._int,
			.rhs = lit_at(parser.current + 2).literal._int,
			.op = '+'
		}
	};
	parser.current += 3;
	return node;
}



static inline token_s peek(void)
{
	return ppeek(1);
}

static inline token_s ppeek(int offset)
{
	if (parser.current + offset >= parser.tokens->final_token ) {
		return token_eof();
	}
	return parser.tokens->token_list[parser.current + offset];
}

static inline token_s token_at(long id)
{
	if (id >= parser.tokens->final_token ) {
		return token_eof();
	}
	return parser.tokens->token_list[id];
}

static inline literal_s lit_at(long tok_id)
{
	if (tok_id >= parser.tokens->final_token) {
		return (literal_s){.type=LIT_NONE};
	}
	int lit_id = parser.tokens->token_list[tok_id].literal_id;	
	return parser.tokens->literal_list[lit_id];
}

void print_ast(node_s tree)
{
	node_s node = tree;
	printf("tree: ( %s )\n", node_to_str(node));
}

static char* node_to_str(node_s node) 
{
	char* output = malloc(sizeof(char) * 4096);
	switch (node.type) {
		case N_INT_LIT:
			sprintf(output, "int: %i\n", node.int_lit);
			return output;
		break;
		case N_B_EXPR:
			sprintf(output, "bexpr: %i '%c' %i", node.bexpr.lhs, node.bexpr.op, node.bexpr.rhs);
			return output;
		break;
		default:
			return "tree";
		break;
	}
}

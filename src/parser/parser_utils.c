
#include "parser.h"
#include "../scanner.h"
#include "parser_utils.h"

struct parser {
	int current;
	file_s source; // current file being parsed
	token_array_s *tokens;
};

struct parser parser;

void init_parser(file_s source, token_array_s *tokens) {
	parser.source = source;
	parser.tokens = tokens;
	parser.current = -1;
}

token_s current_tok(void) { return parser.tokens->token_list[parser.current]; }
int current(void) { return parser.current; }


void consume(void) { parser.current++; }

token_s peek(void) { return peek_n(1); }

token_s peek_n(int offset) {
	if (parser.current + offset > parser.tokens->current_token ) {
		return token_eof();
	}
	return parser.tokens->token_list[parser.current + offset];
}

token_s prev(void) { return prev_n(1); }

token_s prev_n(int offset)
{
	if (parser.current + offset < 0 ) {
		return token_none();
	}
	return parser.tokens->token_list[parser.current - offset];
}

int check_next (token_type expected) { 
	// printf("next is: %s\n", token_to_str(peek().type));
	return check_next_n(1, expected);
}

int check_next_n(int offset, token_type expected) { 
	// printf("T: %i\n", peek_n(offset).type == expected);
	return (peek_n(offset).type == expected); 
}

int match_range(int start, int end) 
{
	if (peek().type >= start && peek().type <= end) {
		consume();
		return 1;
	}
	return 0;
}

int match(token_type expected) {
	if (check_next(expected)) {
		consume();
		return 1;
	}
	return 0;
}

int match_expect(token_type expected)
{
	if (check_next(expected)) {
		consume();
		return 1;
	}
	printf(
			"ERROR: Unexpected token %s at (%i:%i), expected %s\n"
			, token_to_str(peek().type)
			, current_tok().line
			, current_tok().chr_index
			, token_to_str(expected)
			);
	return 0;
}

// void error_unexpected( node_s* node, token_type expected ) {
// 	node->has_error = 1;
// 	node->error = (struct error) {
// 		.token = peek(),
// 		.expected = expected
// 	};
// }

token_s token_at(long id)
{
	if (id >= parser.tokens->current_token ) {
		return token_eof();
	}
	return parser.tokens->token_list[id];
}

literal_s lit_at(long tok_id)
{
	if (tok_id >= parser.tokens->current_token) {
		printf("ERROR: tok_id exceeds total number of tokens\n");
		return (literal_s){.type=LIT_NONE};
	}
	int lit_id = token_at(tok_id).literal_id;	
	return parser.tokens->literal_list[lit_id];
}
